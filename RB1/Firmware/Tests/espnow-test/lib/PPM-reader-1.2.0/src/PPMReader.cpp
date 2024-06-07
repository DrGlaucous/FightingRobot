/*
Copyright 2016 Aapo Nikkilä
Copyright 2021 Dmitry Grigoryev

This file is part of PPM Reader.

PPM Reader is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

PPM Reader is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with PPM Reader.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "PPMReader.h"

PPMReader *PPMReader::ppm;

void PPMReader::PPM_ISR(void) {
  ppm->handleInterrupt(); 
}


PPMReader::PPMReader(byte interruptPin, byte channelAmount, byte isFalling):
    interruptPin(interruptPin), channelAmount(channelAmount) {
    // Setup an array for storing channel values
    rawValues = new unsigned [channelAmount];
    // for (int i = 0; i < channelAmount; ++i) {
    //     rawValues[i] = 0;
    // }
    memset((void*)rawValues, 0, sizeof(unsigned) * channelAmount);
    
    
    partialValues = new unsigned [channelAmount];
    memset((void*)partialValues, 0, sizeof(unsigned) * channelAmount);


    // Attach an interrupt to the pin
    pinMode(interruptPin, INPUT);
    if(ppm == NULL) {
        ppm = this;


    if(isFalling) //weird Futaba 12 channel protocol
        attachInterrupt(digitalPinToInterrupt(interruptPin), PPM_ISR, FALLING);
    else
        attachInterrupt(digitalPinToInterrupt(interruptPin), PPM_ISR, RISING);
    }





}

PPMReader::~PPMReader(void) {
    detachInterrupt(digitalPinToInterrupt(interruptPin));
    if(ppm == this) ppm = NULL;
    delete [] rawValues;
    delete [] partialValues;
}

void PPMReader::handleInterrupt(void) {
    // Remember the current micros() and calculate the time since the last pulseReceived()
    unsigned long previousMicros = microsAtLastPulse;
    microsAtLastPulse = micros();
    unsigned long time = microsAtLastPulse - previousMicros;


    if (time > blankTime) {

        //check for packet completeness
        if(pulseCounter >= channelAmount)
        {
            //unsafe to use memcpy on volatiles; do it manually
            for(int i = 0; i < channelAmount; ++i)
                rawValues[i] = partialValues[i];
        }

        // Blank detected: restart from channel 1 
        pulseCounter = 0;
    }
    else {
        // Store times between pulses as channel values
        if (pulseCounter < channelAmount) {
            partialValues[pulseCounter] = time;
            ++pulseCounter;
        }
    }
}

unsigned PPMReader::rawChannelValue(byte channel) {
    // Check for channel's validity and return the latest raw channel value or 0
    unsigned value = 0;
    if (channel >= 1 && channel <= channelAmount) {
        noInterrupts();
        value = rawValues[channel-1];
        interrupts();
    }
    return value;
}

unsigned PPMReader::latestValidChannelValue(byte channel, unsigned defaultValue) {
    // Check for channel's validity and return the latest valid channel value or defaultValue.
    unsigned value = defaultValue;
	unsigned long timeout;
	noInterrupts();
	timeout = micros() - microsAtLastPulse;
	interrupts();
    if ((timeout < failsafeTimeout) && (channel >= 1) && (channel <= channelAmount)) {
        noInterrupts();
        value = rawValues[channel-1];
		interrupts();
		if (value >= minChannelValue - channelValueMaxError && value <= maxChannelValue + channelValueMaxError) {
			value = constrain(value, minChannelValue, maxChannelValue);
		}
        else value = defaultValue;
    }
    return value;
}
