/*
 Name:		ESP32MIDI.ino
 Created:	5/6/2022 6:17:48 PM
 Author:	Dr. G
*/

#include <Arduino.h>
#include <SPI.h>
#include "configuration.h"
//#include "SendRecTest.h"
#include "SparkfunTest.h"

unsigned long MillisecondTicks{};
unsigned long MicrosecondTicks{};
unsigned long LastMillisecondTicks{};//previous values
unsigned long LastMicrosecondTicks{};

//for STM32 boards, Serial is Serial1
//#ifdef USING_STM32
//MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);
//#else
//MIDI_CREATE_INSTANCE(HardwareSerial, Serial, MIDI);
//#endif


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
    setupFunc();
    //problems();

    //SPI.begin(26, 25, 33, 32);


    Serial.begin(115200);
}


void loop() {

    GetTicks();

    int LiteOn  = false;
    if(MillisecondTicks % 500 == 0 && MillisecondTicks != LastMillisecondTicks)
    {
        LiteOn = true;//only for one tick

        //digitalWrite(LED_BUILTIN, LiteOn);
        Serial.println(LiteOn);

    }

    loopFunc(LiteOn);
    //loopFunc(LiteOn);
    //Serial.print(LiteOn);



}

