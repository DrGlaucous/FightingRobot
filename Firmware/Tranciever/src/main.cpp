
#include <Arduino.h>

#include "configuration.h"
#include "TimerTick.h"

#ifdef IS_CONTROLLER
#include "Transmitter.h"
TransmitterHandler gTransmitter;
#else
#include "Receiver.h"
ReceiverHandler gReceiver;
#endif



void setup() {

    //nothing to do here right now

#ifdef MODE_DEBUG
    Serial.begin(115200);
#endif
}


void loop() {

    gTimer.update();

#ifdef IS_CONTROLLER
    gTransmitter.update();
#else
    gReceiver.update();
#endif


}

