#include <Arduino.h>
#include <SPI.h>


#include "configuration.h"
#include "RFM69HW.h"


RadioHandler radio;

void onPacketGet() {

    Packet got_packet = {};
    ptype type = {};
    if(radio.checkForPackets(got_packet, type)) {
        Serial.printf("Got packet of type: %d\n", type);

        if(type == CONTROL_TYPE) {            
            Serial.printf("Analog Channel 0: %d\n", got_packet.control.analog_channels[0]);
        } else {
            Serial.printf("Response voltage: %f\n", got_packet.response.battery_voltage);
        }
    }

}


void setup() {

    //open serial for debugging
    Serial.begin(115200);


#ifdef USING_ESP32
    SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI);
#elif USING_STM32
    SPI.begin();
#endif

    radio.begin(&SPI, PIN_NSS, PIN_RESET, PIN_ISR, MYNODEID, TONODEID, ENCRYPTKEY);

    //runs when we get a packet
    radio.setCustomRecCallback(onPacketGet);


    //stm32 tests:
    pinMode(PC13, OUTPUT);
    digitalWrite(PC13, LOW);

}


bool flip = false;
int last_ms = 0;
void loop() {

    // while(1) {
    //     digitalWrite(PC13, LOW);
    //     delay(1000);
    //     digitalWrite(PC13, HIGH);
    //     delay(1000);
    // }


    if(millis() - last_ms >= 1000) {
        flip = !flip;
        last_ms = millis();
        Serial.printf("Hello, there.\n");

        Packet packet = {};
        if(flip) {
            packet.control.analog_channels[0] = -336;
            packet.control.digital_switches[0] = 1;
            radio.sendPacket(&packet, CONTROL_TYPE, TONODEID, USEACK);

            digitalWrite(PC13, LOW);


        } else {
            packet.response.battery_voltage = 44.312;
            radio.sendPacket(&packet, RESPONSE_TYPE, TONODEID, USEACK);

            digitalWrite(PC13, HIGH);
        }     

    }

    delay(1);

}

