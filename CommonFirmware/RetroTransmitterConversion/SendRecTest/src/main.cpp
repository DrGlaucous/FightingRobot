#include <Arduino.h>
#include "SparkfunTest.h"

uint32_t MillisecondTicks = 0;

void setup() {
    setupFunc();
}


void loop() {


    int LiteOn  = false;
    if(millis() - MillisecondTicks > 1000)
    {
        MillisecondTicks = millis();
        LiteOn = true;//only for one tick
    }

    loopFunc(LiteOn);

    delay(1);

}

