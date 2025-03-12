#include <Arduino.h>

// The RMT (Remote Control) module library is used for generating the DShot signal.
#include <driver/rmt_rx.h>
#include <driver/rmt_tx.h>

//less universal now, but can be undone if I *really* need it for something else...
#include "configuration.h"
#include "PPMRMT.h"



//ripped straight from dshot stuffs
constexpr auto RMT_CLK_DIVIDER = 80;    // Slow down RMT clock to 0.1 microseconds / 100 nanoseconds per cycle [8], trying 80, slowed down to 1microsecond per cycle
constexpr auto F_CPU_RMT = APB_CLK_FREQ;
constexpr auto RMT_CYCLES_PER_SEC = (F_CPU_RMT / RMT_CLK_DIVIDER);
constexpr auto RMT_CYCLES_PER_ESP_CYCLE = (F_CPU / RMT_CYCLES_PER_SEC); //not used ATM



//callbacks need to be in C and not part of any class
extern "C"
{

//ensures that the rx callback code is always in iram, which is essential for speed
#define CONFIG_RMT_ISR_IRAM_SAFE 1
#if CONFIG_RMT_ISR_IRAM_SAFE
#define TEST_RMT_CALLBACK_ATTR IRAM_ATTR
#else
#define TEST_RMT_CALLBACK_ATTR
#endif

//callback when we get data back in
//flag the selected code to be always loaded into RAM
TEST_RMT_CALLBACK_ATTR
static bool rx_done_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *edata, void *user_data)
{
	//init return value
	//high task wakeup will be true if pushing data to the queue started a task with a higher priority than this inturrupt.
    BaseType_t high_task_wakeup = pdFALSE;

	//get pointer to the settings passed in by us when we initialized the callback
	ppm_rx_callback_datapack_t* config = (ppm_rx_callback_datapack_t*)user_data;

	ppm_rx_frame_data_t out_data = {};

	//copy the edata symbols to the frame data
	size_t sym_count = edata->num_symbols > PPM_RX_SYMBOL_MAX ? PPM_RX_SYMBOL_MAX : edata->num_symbols; //cap copy size to 16

    //only update the queue if we got the correct number of channels (the radio didn't cut out partway through a transmission)
    if(sym_count > config->channel_count)
    {
        memcpy(&out_data.received_symbols, edata->received_symbols, sym_count * sizeof(rmt_symbol_word_t));
        out_data.num_symbols = sym_count;
        
        //send the received RMT symbols to the parser task
	    xQueueOverwriteFromISR(config->receive_queue, &out_data, &high_task_wakeup);
    }

	//TEST
	//Serial.printf("~%d\n", edata->num_symbols);
	//for(int i = 0; i < edata->num_symbols; ++i)
	//	Serial.printf("%d,%d|%d,%d\n",edata->received_symbols[i].duration0,edata->received_symbols[i].level0,
	//			edata->received_symbols[i].duration1,edata->received_symbols[i].level1);


	// size_t last_sym = edata->num_symbols - 1;
	// if(edata->received_symbols[last_sym].duration0 != 0
	// 	&& edata->received_symbols[last_sym].duration1 != 0)
	// {
	// 	Serial.printf("~\n");
	// 	//out_data.non_termination = true;
	// }

    //start listening for a response (again)
    rmt_receive(config->rx_chan, config->raw_symbols, 64 * sizeof(rmt_symbol_word_t), config->rx_config);

    return high_task_wakeup == pdTRUE; //return xQueueSendFromISR result (does it wake up a higher task?)
}


}


PPMRMT::PPMRMT(uint8_t s_pin, uint16_t s_channel_count)
{
    pin = s_pin;
    channel_count = s_channel_count;

    //holder for rx channel settings until we install them
    rmt_rx_channel_config_t rx_chan_config = 
    {
        .gpio_num = (gpio_num_t)pin,
        .clk_src = RMT_CLK_SRC_DEFAULT, // select a clock that can provide needed resolution (apb)
        .resolution_hz = RMT_CYCLES_PER_SEC, // esc resolution in Hz
        .mem_block_symbols = 64, // default count per channel
        .flags =
            {
            .invert_in = 0, //don't invert the input logic
            .io_loop_back = 0,	//don't enable loopback			
            }
    };

	//populate channel object
    handle_error(rmt_new_rx_channel(&rx_chan_config, &rx_chan));

    //configure recieve callbacks
    rmt_rx_event_callbacks_t callback = 
    {
        .on_recv_done = rx_done_callback
    };  

    // create a thread safe queue handle
	receive_queue = xQueueCreate(1, sizeof(ppm_rx_frame_data_t));

	//register them
	handle_error(rmt_rx_register_event_callbacks(rx_chan, &callback, &rx_callback_datapack));

    //frame count * (ticks / 10) * 1000 for ms
    //each tick is worth 0.1 us in the rmt backend (100 nanos) (was with div/8), using div/80
    //for futaba 12 ch PPM ONLY:
    //shortest time is 50 micros, 50 ticks (waay smaller than typical PPM signal)
    //longest time is 2000

    //these go into the rx callback datapack so it can restart itself.
    rx_config =
	{
		.signal_range_min_ns=PPM_MIN_WAIT_VALUE,
		.signal_range_max_ns=PPM_BLANK_TIME
	};

	//stow settings into datapack so we can get them in the callback
	rx_callback_datapack.receive_queue = receive_queue;
    rx_callback_datapack.rx_config = &rx_config;
    rx_callback_datapack.raw_symbols = ppm_rx_rmt_item;
    rx_callback_datapack.rx_chan = rx_chan;
    rx_callback_datapack.channel_count = channel_count;

    Serial.println("t");

    handle_error(rmt_enable(rx_chan));


    //start listening for a response
    rmt_receive(rx_callback_datapack.rx_chan, rx_callback_datapack.raw_symbols, 64 * sizeof(rmt_symbol_word_t), rx_callback_datapack.rx_config);

    Serial.println("u");

}

PPMRMT::~PPMRMT()
{
    rmt_del_channel(rx_chan);
    vQueueDelete(receive_queue);
}

bool PPMRMT::check_ppm_queue()
{
    if(xQueueReceive(receive_queue, &queue_dump, 0) == pdTRUE)
    {

        //Serial.printf("Got %d syms\n", queue_dump.num_symbols);
        // Serial.println("===============================");
        // for (int i = 0; i < queue_dump.num_symbols; ++i)
        // {
        //     char hold[100] = {};
        //     sprintf(hold, "D0: %d L0: %d || D1: %d L1: %d",
        //     queue_dump.received_symbols[i].duration0, queue_dump.received_symbols[i].level0,
        //     queue_dump.received_symbols[i].duration1, queue_dump.received_symbols[i].level1);
        //     Serial.println(hold);
        // }
        // Serial.println("===============================");
        // for(int i = 0; i < queue_dump.num_symbols; ++i)
        // {
        //     Serial.printf(" %d :%4d ||", i, queue_dump.received_symbols[i].duration0 + queue_dump.received_symbols[i].duration1);
        // }
        // Serial.printf("\n");

        //transfer values to the value holder (-1 because there is an extra symbol on the tailing end that gets discarded)
        for(int i = 0; i < queue_dump.num_symbols - 1; ++i)
        {
            gotten_ppms[i] = queue_dump.received_symbols[i].duration0 + queue_dump.received_symbols[i].duration1;
        }
        return true;

    }
    return false;

}

uint32_t PPMRMT::get_latest_ppm(uint16_t adr_channel)
{
    //OOB catcher
    if(adr_channel >= channel_count)
    {
        adr_channel = channel_count -1;
    }

    //note that channel array is NOT base 1 (even though channel IDs are base-1)
    return gotten_ppms[adr_channel];

}


void PPMRMT::handle_error(esp_err_t err_code) {
	if (err_code != ESP_OK) {
		Serial.print("error: ");
		Serial.println(esp_err_to_name(err_code));
	}
}

