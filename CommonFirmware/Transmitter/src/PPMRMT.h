#pragma once

#include <Arduino.h>

// The RMT (Remote Control) module library is used for generating the DShot signal.
#include <driver/rmt_rx.h>
#include <driver/rmt_tx.h>

//less universal now, but can be undone if I *really* need it for something else...
#include "configuration.h"


//12 channels, 2 extra holes just in case
#define PPM_RX_SYMBOL_MAX PPM_CHANNEL_COUNT + 2

//a lot of this I'm stealing from the dshot driver... it's useful



//holds everything the RX callback needs to do its thing
typedef struct ppm_rx_callback_datapack_s
{
    //thread-safe queue object that the RX callback uses
    QueueHandle_t receive_queue;

    //pointer to the config to be used to start the rx listener again
    rmt_receive_config_t* rx_config;

    //pointer to where the symbols go when gotten
    rmt_symbol_word_t *raw_symbols;

    //handle to the rmt rx channel
    rmt_channel_handle_t rx_chan;

    uint16_t channel_count;


} ppm_rx_callback_datapack_t;


//holds the actual frame data received from the remote, this is what's passed back and forth through the xQueue
typedef struct ppm_rx_frame_data_s
{
    size_t num_symbols; //how many of the 11 frames are filled with data
    rmt_symbol_word_t received_symbols[PPM_RX_SYMBOL_MAX];

}ppm_rx_frame_data_t;

// typedef enum ppm_poll_result_e
// {
//     UPDATE_GOOD,
//     ERR_PPM_OUT_OF_RANGE, 
//     ERR_NO_FRESH_PACKET,
// } ppm_poll_result_t;


class PPMRMT
{
    public:

    PPMRMT(uint8_t s_pin, uint16_t s_channel_count);
    ~PPMRMT();

    //returns TRUE if more data is waiting in the queue
    bool check_ppm_queue();

    //returns the last cached PPM value
    uint32_t get_latest_ppm(uint16_t channel);

    private:

    void handle_error(esp_err_t err_code);

    //thread-safe queue object that the RX callback uses
    QueueHandle_t receive_queue;

    //how many channels are to be expected in a "full" packet
    uint16_t channel_count = {};

    //RMT channel pin
    uint8_t pin = {};
    //channel config (things like GPIO number send buffer, clock resolution live here)
    rmt_channel_handle_t rx_chan;
    //configuration for starting rx channel
    rmt_receive_config_t rx_config;


    ppm_rx_callback_datapack_t rx_callback_datapack; //collection of settings passed to the rx callback

    
    //new data will be dumped in here from the callback (size doesn't need to be exact, we make it bigger to be safe)
    //(I think it isn't thread safe to touch this while RMT callbacks are running)
    rmt_symbol_word_t ppm_rx_rmt_item[64] = {};

    ppm_rx_frame_data_t queue_dump = {};

    //array of correct PPM values
    uint32_t gotten_ppms[PPM_RX_SYMBOL_MAX] = {};


};



