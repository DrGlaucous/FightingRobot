
#include <Arduino.h>

#include <esp_now.h>
#include <esp_wifi.h>
#include <Wifi.h>

#include "configuration.h"
#include "TimerTick.h"
#include "Transmitter.h"
#include "PPMRMT.h"


TransmitterHandler* transmitter;


void setup()
{
    Serial.begin(115200);

    transmitter = new TransmitterHandler();
}


void loop()
{
    transmitter->update();
}





