
#include <Arduino.h>
#include "configuration.h"
#include "Radio.h"

unsigned long MillisecondTicks{};
unsigned long MicrosecondTicks{};
unsigned long LastMillisecondTicks{};//previous values
unsigned long LastMicrosecondTicks{};


RadioHandler* transmitter;




//System clock
void GetTicks(void)
{
	LastMillisecondTicks = MillisecondTicks;
	LastMicrosecondTicks = MicrosecondTicks;

	MillisecondTicks = millis();
	MicrosecondTicks = micros();

}



void setup() {

    //pinMode(LED_BUILTIN, OUTPUT);


    auto startup_settings = radio_handler_config_datapack_t
    {
        SLAVE_PIN,
        IRQ_PIN,
        FREQUENCY,
        MYNODEID,
        NETWORKID,
        IS_HIGH_POWER,
        ENCRYPT,
        ENCRYPTKEY,
    };
    transmitter = new RadioHandler(startup_settings);
}


void loop() {

    GetTicks();

    int LiteOn  = false;
    if(MillisecondTicks % 500 == 0 && MillisecondTicks != LastMillisecondTicks)
    {
        LiteOn = true;//only for one tick

        remote_control_packet_t packet_out = {};
        packet_out.channels[0] = 1280;
        
        transmitter->SendRCPacket(packet_out, TONODEID, USEACK);

    }

    packet_type_t gpack = PACKET_NULL;
    transmitter->CheckForResponse(&gpack);

    if(gpack != PACKET_NULL)
        Serial.printf("%d\n", transmitter->GetLastControlPacket().channels[0]);

}

