
#include <Arduino.h>
#include <PPMReader.h>


#include "configuration.h"
#include "TimerTick.h"
#include "Radio.h"
#include "Controller.h"


//Initialize a PPMReader on digital pin PA0 with 12 expected channels.
//uint32_t interruptPin = PA0;
//byte channelAmount = 12;
//PPMReader ppm(PPM_INTURRUPT_PIN, PPM_CHANNEL_COUNT, PPM_IS_INVERTED);



//test, visual feedback of a recieved packet
void blink_lt(unsigned int time)
{
    static unsigned long last_time = 0;
    static unsigned long delta_time = 0;

    if(time != 0)
    {
        digitalWrite(LED_BUILTIN, LOW); //on
        delta_time = time;
    }

    if(gTimer.DeltaTimeMillis(&last_time, delta_time))
    {
        digitalWrite(LED_BUILTIN, HIGH); //off
    }

}


RadioHandler* radio;
ControllerHandler* controller;

void setup() {


    auto startup_settings = radio_handler_config_datapack_t
    {
        SLAVE_PIN,
        IRQ_PIN,
        FREQUENCY,

#ifdef IS_CONTROLLER
        TRANSMITTERNODEID,
#else
        RECEIVERNODEID,
#endif
        NETWORKID,
        IS_HIGH_POWER,
        ENCRYPT,
        ENCRYPTKEY,
    };
    radio = new RadioHandler(startup_settings);
    
    
    controller = new ControllerHandler();

    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);

    // //setup PPM values for 12 channel mode
    // ppm.blankTime = 5000;//10000;
    // ppm.maxChannelValue = 1100;
    // ppm.minChannelValue = 400;

}


void loop() {

    gTimer.update();

    blink_lt(0);

    static unsigned long last = 0;
#ifdef IS_CONTROLLER
    if(gTimer.DeltaTimeMillis(&last, 5))
    {

        //moved all this to the dedicated class:

        //read PPM into packet struct
        // for (byte channel = 1; channel <= PPM_CHANNEL_COUNT; ++channel) {
        //     //note: ppm.latestValidChannelValue is base-1!
        //     packet_out.channels[channel - 1] = ppm.latestValidChannelValue(channel, 0);
        // }
        //Serial.println(packet_out.channels[0]);
        // Print latest valid values from all channels
        // for (byte channel = 1; channel <= channelAmount; ++channel) {
        //     unsigned value = ppm.latestValidChannelValue(channel, 0);
        //     //unsigned value = ppm.rawChannelValue(channel);
        //     Serial.print(value);
        //     if(channel < channelAmount) Serial.print('\t');
        // }
        // Serial.println();
        blink_lt(10);

        controller->update();



        //transmit packet

        //ready packet
        remote_control_packet_t packet_out = {};
        packet_out.channels[0] = 1280;

        //if transmit success print the bounceback time
        if(!radio->SendPacket(packet_out, RECEIVERNODEID, USEACK))
        {
            Serial.printf("%d\n", radio->GetLastResponsePacket().hi_there);
        }

    }
#else



    remote_ack_packet_t ackpt = {};
    //last += 1;
    ackpt.hi_there = radio->GetLastControlPacket().channels[0];
    response_status_t responsee = radio->CheckForResponse(NULL, ackpt);
    if(responsee == RX_SENT_ACK || responsee == RX_SUCCESS)
    {
        blink_lt(100);
    }

#endif






}





