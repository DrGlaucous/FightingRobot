#include <Arduino.h>
#include <PPMReader.h>

#include "TimerTick.h"
#include "Receiver.h"
#include "configuration.h"

#ifndef IS_CONTROLLER

ReceiverHandler::ReceiverHandler()
{
        //start radio transmitter/receiver
    auto startup_settings = radio_handler_config_datapack_t
    {
        SLAVE_PIN,
        IRQ_PIN,
        FREQUENCY,
        RECEIVERNODEID,
        NETWORKID,
        IS_HIGH_POWER,
        ENCRYPT,
        ENCRYPTKEY,
    };
    radio = new RadioHandler(startup_settings);

}

ReceiverHandler::~ReceiverHandler()
{
    delete(radio);
}



void ReceiverHandler::update()
{
    if(gTimer.MillisChanged() && gTimer.DeltaTimeMillis(&last_sent, millis_time))
    {

#ifdef MODE_DEBUG
        Serial.println("Sent packet.");
#endif

        //get latest telemetry data
        packet_out.hi_there != packet_out.hi_there; 

        //send that data off to the controller
        radio->SendResponsePacket(packet_out, TRANSMITTERNODEID, USEACK);

    }


    //check for commands from the controller
    packet_type_t gpack = PACKET_NULL;
    radio->CheckForResponse(&gpack);


    //perform actions based on commands (right now, print telemetry to command window)
    switch(gpack)
    {
        PACKET_NULL:
            break;
        PACKET_CONTROL_VALUES:
            //reset keepalive
            last_rx_time = millis();

            packet_in = radio->GetLastControlPacket();
            Serial.printf("%d\n", packet_in.channels[0]);

            //this is where robot control surfaces should be updated

            break;
        PACKET_RESPONSE_VALUES: //should never get these
            Serial.printf("%d\n", radio->GetLastResponsePacket().hi_there);
            break;
    }



}




#endif





