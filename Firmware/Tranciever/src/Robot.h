#pragma once
#include <Arduino.h>

#include "Controller.h"
#include "Radio.h"
#include "configuration.h"
#include "TimerTick.h"


#ifndef IS_CONTROLLER



//test class for light blinking

class BlinkerHandler
{
    public:

    BlinkerHandler(int pin_no)
    {
        pinMode(pin_no, OUTPUT);
        lite_pin = pin_no;
    }

    
    ~BlinkerHandler()
    {

    }


    void update()
    {   
        if(gTimer.DeltaTimeMillis(&last_time, delta_time))
        {
            digitalWrite(LED_BUILTIN, HIGH); //off
        }
    }

    void blink_lt(unsigned int time)
    {
        digitalWrite(LED_BUILTIN, LOW); //on
        delta_time = time;
    }


    private:

    int lite_pin = 0;
    unsigned long last_time = 0;
    unsigned long delta_time = 0;

};



//does all the roboty things (like movement, weapon handling, etc.)
class RobotHandler
{

    public:

    RobotHandler();
    ~RobotHandler();

    void update();


    private:

    //for timing receptions
    unsigned long last_time = 0;

    RadioHandler* radio;

    //BlinkerHandler* blinker;


};




#endif

