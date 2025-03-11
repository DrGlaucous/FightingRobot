
#include <Arduino.h>

#include <esp_now.h>
#include <esp_wifi.h>
#include <Wifi.h>

#include "configuration.h"
#include "TimerTick.h"
#include "ESPRadio.h"

RadioNowHandler* rhandler = nullptr; //RadioNowHandler();

const char payload[] = "Payload\0";

void setup()
{
    Serial.begin(115200);
    Serial.printf("Start");

    rhandler = new RadioNowHandler();

}

uint32_t last_millis = 0;

void loop()
{
    if(millis() - last_millis > 2000) {
        auto status = rhandler->SendPacket(payload, sizeof(payload));
        Serial.printf("Send Status: %d\n", status);
        last_millis = millis();
    }

    packet_type_t packet_type = {};
    if(rhandler->CheckForPacket(&packet_type) == RX_SUCCESS) {
        Serial.printf("Got\n");
    }
}





