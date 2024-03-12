#include <Arduino.h>
#include <PPMReader.h>


#include "configuration.h"
#include "Controller.h"
#include "TimerTick.h"
#include "Transmitter.h"
#include "ESPRadio.h"

#ifdef IS_CONTROLLER


TransmitterHandler::TransmitterHandler()
{
    radio = new RadioNowHandler();
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

        //if(controller->update())
        //   return;
        
        //test: ignore null PPM
        //controller->update();


        //ready the packet
        remote_control_packet_t packet_out = {};
        packet_out.channels = controller->GetReadyPacket();


        // test: check for error values (we seem to be getting OOB vals on channel 3...)
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



        // if transmit success, print the bounceback packet
        if(radio->SendPacket(packet_out) == TX_SUCCESS)
        {
            //sprinf(%f) is not supported with the current STM32 backend... cast to int
            // float num = radio->GetLastResponsePacket().battery_voltage;
            // u_int32_t decimal_ten = 1000; //3 decimals
            // Serial.printf( "%d.%d\n", (int)num, (int)(num * 1000) % 1000);
            //Serial.printf("Sent Success\n");
        }



    }
}


#endif

