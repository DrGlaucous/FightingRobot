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

#define MOT_A1 22
#define MOT_A2 19
#define MOT_B1 18
#define MOT_B2 5

void setup() {

    //pinMode(LED_BUILTIN, OUTPUT);

    pinMode(MOT_A1, OUTPUT);
    pinMode(MOT_A2, OUTPUT);
    pinMode(MOT_B1, OUTPUT);
    pinMode(MOT_B2, OUTPUT);

    setupFunc();
    //problems();

    //SPI.begin(26, 25, 33, 32);


    Serial.begin(115200);
}


void loop() {

    GetTicks();

    static int LiteOn  = 0;
    if(MillisecondTicks % 500 == 0 && MillisecondTicks != LastMillisecondTicks)
    {
        ++LiteOn;// = true;//only for one tick

        //digitalWrite(LED_BUILTIN, LiteOn % 2);
        Serial.println(LiteOn);

        digitalWrite(MOT_A1, LiteOn % 4);
        digitalWrite(MOT_A2, !(LiteOn % 4));
        digitalWrite(MOT_B1, LiteOn % 4);
        digitalWrite(MOT_B2, !(LiteOn % 4));

    }

    loopFunc(LiteOn);
    //loopFunc(LiteOn);
    //Serial.print(LiteOn);



}

