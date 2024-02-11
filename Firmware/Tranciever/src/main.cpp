
#include <Arduino.h>

#include "configuration.h"
#include "TimerTick.h"
#include "Radio.h"



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

    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);

}


void loop() {

    gTimer.update();

    blink_lt(0);

#ifdef IS_CONTROLLER
    static unsigned long last = 0;
    if(gTimer.DeltaTimeMillis(&last, 5))
    {
        remote_control_packet_t packet_out = {};
        packet_out.channels[0] = 1280;

        radio->SendPacket(packet_out, RECEIVERNODEID, USEACK);
    }
#endif

    if(!radio->CheckForResponse())
    {
        blink_lt(10);
    }





}





