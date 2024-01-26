#include <Arduino.h>
#include <PPMReader.h>

#include "TimerTick.h"
#include "Transmitter.h"
#include "configuration.h"

#ifdef IS_CONTROLLER

TransmitterHandler::TransmitterHandler()
{

    //start radio transmitter/receiver
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
    };
    radio = new RadioHandler(startup_settings);

    //start remote control reader
    ppm = new PPMReader(PPM_INTURRUPT_PIN, PPM_CHANNEL_COUNT, PPM_IS_INVERTED);

    ppm->blankTime = PPM_BLANK_TIME; //10000; // was for 8 channel "traditional PPM"
    ppm->maxChannelValue = PPM_MAX_WAIT_VALUE;
    ppm->minChannelValue = PPM_MIN_WAIT_VALUE;

}

TransmitterHandler::~TransmitterHandler()
{
    delete(radio);
    delete(ppm);
}

void TransmitterHandler::update()
{
    if(gTimer.MillisChanged() && gTimer.DeltaTimeMillis(&last_sent, millis_time))
    {
        
#ifdef MODE_DEBUG
        Serial.println("Sent packet.");
#endif

        //get latest control surface data
        for(byte i = 0; i < CHANNEL_COUNT; ++i)
        {
            packet_out.channels[i] = ppm->latestValidChannelValue(i, packet_out.channels[i]);
        }

        //send that data off to the robot
        radio->SendRCPacket(packet_out, RECEIVERNODEID, USEACK);

    }


    //check for telemetry back from the robot
    packet_type_t gpack = PACKET_NULL;
    radio->CheckForResponse(&gpack);


    //perform actions based on telemetry (right now, print telemetry to command window)
    switch(gpack)
    {
        PACKET_NULL:
            break;
        PACKET_CONTROL_VALUES: //should never get these
            Serial.printf("%d\n", radio->GetLastControlPacket().channels[0]);
            break;
        PACKET_RESPONSE_VALUES:
            packet_in = radio->GetLastResponsePacket();
            Serial.printf("%d\n", packet_in.hi_there);
            break;
    }



}




#endif




