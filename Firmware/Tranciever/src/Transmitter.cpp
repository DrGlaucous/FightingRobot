#include <Arduino.h>
#include <PPMReader.h>


#include "configuration.h"
#include "Radio.h"
#include "Controller.h"
#include "TimerTick.h"
#include "Transmitter.h"

#ifdef IS_CONTROLLER



TransmitterHandler::TransmitterHandler()
{
#ifdef USING_ESP32
    //set up SPI matrix on ESP32 (can use non-default pins)
    SPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN, SPI_NSS_PIN);
#endif

    auto startup_settings = radio_handler_config_datapack_t
    {
        SLAVE_PIN,
        IRQ_PIN,
        FREQUENCY,
        TRANSMITTERNODEID,
        NETWORKID,
        IS_HIGH_POWER,
        ENCRYPT,
        ENCRYPTKEY,
        &SPI,
    };
    radio = new RadioHandler(startup_settings);

    controller = new ControllerHandler();

}

TransmitterHandler::~TransmitterHandler()
{
    delete(radio);
    delete(controller);
}


void TransmitterHandler::update()
{
    //5 ms have elapsed
    if(gTimer.DeltaTimeMillis(&last_time, 5))
    {

        if(controller->update())
            return;


        //ready the packet
        remote_control_packet_t packet_out = {};
        packet_out.channels = controller->GetReadyPacket();


        // test: check for error values (we seem to be getting those)
        // packet_out.channels.analog_channels[3] = 255;
        // bool wrongful = false;
        // for(int i = 0; i < ANALOG_CHANNEL_CNT; ++i)
        // {
        //     if(packet_out.channels.analog_channels[i] > 300 || packet_out.channels.analog_channels[i] < 240)
        //     {
        //         wrongful = true;
        //         Serial.printf("%d || ", packet_out.channels.analog_channels[i]);
        //     }
        // }
        // if(wrongful)
        //     Serial.printf("\n");


        //if transmit success, print the bounceback packet
        if(!radio->SendPacket(packet_out, RECEIVERNODEID, USEACK))
        {
            Serial.printf("%f\n", radio->GetLastResponsePacket().battery_voltage);
        }
    }
}


#endif

