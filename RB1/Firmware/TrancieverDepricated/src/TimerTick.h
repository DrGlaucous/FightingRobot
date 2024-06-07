//global timer class: makes getting delta time easier
#pragma once
#include <Arduino.h>

#include "configuration.h"


class TimerHandler
{

public:

    //update clock
    void update(void)
    {
        LastMillisecondTicks = MillisecondTicks;
        LastMicrosecondTicks = MicrosecondTicks;

        MillisecondTicks = millis();
        MicrosecondTicks = micros();

    }

    //true if changed
    inline bool MillisChanged()
    {
        return LastMillisecondTicks != MillisecondTicks;
    }
    inline bool MicrosChanged()
    {
        return LastMicrosecondTicks != MicrosecondTicks;
    }

    //true if time >= last time
    inline bool DeltaTimeMillis(unsigned long* last_time, unsigned long elapse_time)
    {
        return DeltaTimeUnit(last_time, elapse_time, MillisecondTicks);
    }
    inline bool DeltaTimeMicros(unsigned long* last_time, unsigned long elapse_time)
    {
        return DeltaTimeUnit(last_time, elapse_time, MicrosecondTicks);
    }



private:

    unsigned long MillisecondTicks{};
    unsigned long MicrosecondTicks{};
    unsigned long LastMillisecondTicks{};//previous values
    unsigned long LastMicrosecondTicks{};

    bool DeltaTimeUnit(unsigned long* last_time, unsigned long elapse_time, unsigned long counter)
    {
        if(*last_time + elapse_time <= counter)
        {
            *last_time = counter;
            return true;
        }
        return false;
    }

};


extern TimerHandler gTimer; 

