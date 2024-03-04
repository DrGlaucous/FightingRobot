/*
 Name:		ESP32MIDI.ino
 Created:	5/6/2022 6:17:48 PM
 Author:	Dr. G
*/

#include <Arduino.h>
#include <PPMReader.h>

unsigned long MillisecondTicks{};
unsigned long MicrosecondTicks{};
unsigned long LastMillisecondTicks{};//previous values
unsigned long LastMicrosecondTicks{};

//Initialize a PPMReader on digital pin 3 with 6 expected channels.

#ifdef USING_STM32
#define IRQ_PIN PA0
#elif USING_ARDUINO
//non-analog pins can be delcared using digits in arduino
#define IRQ_PIN 2
#else
#define IRQ_PIN -1
#endif

uint32_t interruptPin = IRQ_PIN;

byte channelAmount = 12;
PPMReader ppm(interruptPin, channelAmount, 1);


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

    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);

    ppm.blankTime = 5000;//10000;
    ppm.maxChannelValue = 1100;
    ppm.minChannelValue = 400;

}


void loop() {

    // GetTicks();

    // static int LiteOn  = false;
    // if(MillisecondTicks % 500 == 0 && MillisecondTicks != LastMillisecondTicks)
    // {
    //     LiteOn = !LiteOn;

    //     digitalWrite(LED_BUILTIN, LiteOn);
    //     //Serial.printf("Tick\n");

    // }




    // Print latest valid values from all channels
    for (byte channel = 1; channel <= channelAmount; ++channel) {
        unsigned value = ppm.latestValidChannelValue(channel, 0);
        //unsigned value = ppm.rawChannelValue(channel);

        Serial.print(value);
        if(channel < channelAmount) Serial.print('\t');
    }
    Serial.println();
    delay(20);



}

