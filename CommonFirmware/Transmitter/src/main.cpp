
#include <Arduino.h>

#include <esp_now.h>
#include <esp_wifi.h>
#include <Wifi.h>

#include "configuration.h"
#include "TimerTick.h"
#include "Transmitter.h"
#include "PPMRMT.h"

//custom MAC addresses
//uint8_t sender_addr[] = {0x30, 0xAE, 0xA4, 0x07, 0x0D, 0x64};
//uint8_t rec_addr[] = {0xA0, 0x0E, 0x04, 0x0F, 0xFD, 0x64};



TransmitterHandler* transmitter;

//PPMRMT* ppm_get;

void setup()
{
    Serial.begin(115200);

    transmitter = new TransmitterHandler();
}


void loop()
{

    transmitter->update();

}





