
#include <Arduino.h>


#include "configuration.h"
#include "TimerTick.h"


TimerHandler gTimer; 

void setup()
{

    Serial.begin(115200);
    pinMode(MOTOR_3A_PIN, OUTPUT);
    pinMode(MOTOR_3B_PIN, OUTPUT);

    pinMode(MOTOR_SLEEP_PIN, OUTPUT);

    digitalWrite(MOTOR_3A_PIN, true);
    digitalWrite(MOTOR_3B_PIN, false);

    digitalWrite(MOTOR_SLEEP_PIN, false);

}


void loop()
{

    gTimer.update();

    static unsigned long delta_t = 0;
    static unsigned int l_state = 0;
    if(gTimer.DeltaTimeMillis(&delta_t, 500))
    {
        digitalWrite(MOTOR_3A_PIN, l_state % 2);
        ++l_state;

        Serial.printf("beat %d\n", l_state);

    }


}





