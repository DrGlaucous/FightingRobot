#include <Arduino.h>

#include "Controller.h"
#include "Radio.h"
#include "configuration.h"
#include "Robot.h"

#ifndef IS_CONTROLLER




RobotHandler::RobotHandler()
{
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

    //blinker = new BlinkerHandler(LED_BUILTIN);
}


RobotHandler::~RobotHandler()
{
    delete(radio);
    //delete(blinker);
}



void RobotHandler::update()
{
    //blinker->update();

    if(gTimer.DeltaTimeMillis(&last_time, 1000))
    {
        //blinker->blink_lt(100);
    }

    auto gotten_data = radio->GetLastControlPacket().channels;

    //prepare potenial response with latest data
    remote_ack_packet_t ackpt = {};
    ackpt.hi_there = gotten_data.analog_channels[0];
    //ackpt.hi_there = radio->GetLastControlPacket().channels[0];

    //sniff for packets
    response_status_t responsee = radio->CheckForResponse(NULL, ackpt);

    //blink basd on what we got
    if(responsee == RX_SENT_ACK || responsee == RX_SUCCESS)
    {
        //blinker->blink_lt(100);
        

        //dump recieved channels
        for(int i = 0; i < ANALOG_CHANNEL_CNT; ++i)
        {
            Serial.printf("%d : %d||", i, gotten_data.analog_channels[i]);
        }
        Serial.printf("|//|");
        for(int i = 0; i < DIGITAL_CHANNEL_CNT * 2; ++i)
        {
            Serial.printf("%d : %d||", i, gotten_data.digital_channels[i]);
        }
        Serial.printf("\n");


    }

}




#endif

