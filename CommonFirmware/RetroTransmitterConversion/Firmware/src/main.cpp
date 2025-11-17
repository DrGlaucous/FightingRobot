#include <Arduino.h>
#include <SPI.h>


#include "configuration.h"
#include "RFM69HW.h"


RadioHandler* rh = nullptr;


void setup() {

    Serial.begin(115200);

#ifdef USING_ESP32
    SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI);
#elif USING_STM32
    SPI.begin();
#endif



    rh = new RadioHandler();

    rh->begin(&SPI, PIN_NSS, PIN_RESET, PIN_ISR, MYNODEID, NETWORKID, ENCRYPTKEY);




}

bool flipper = false;
int last_ms = 0;
void loop() {


    if(millis() - last_ms >= 10) {
        last_ms = millis();
        flipper = !flipper;

        Serial.printf("Send\n");


        Packet pt = {};
        ptype ptt = CONTROL_TYPE;
        if(flipper) {


            pt.control.analog_channels[0] = 67;
            ptt = CONTROL_TYPE;            

        } else {

            pt.response.battery_voltage = 6.7;
            ptt = RESPONSE_TYPE;
        }

        rh->sendPacket(&pt, ptt, TONODEID, false);



    }


    Packet pt = {};
    ptype ptt = {};
    if(rh->checkForPackets(pt, ptt)) {
        //nothing for now.
        if(ptt == CONTROL_TYPE) {
            Serial.printf("Analog channel 0: %d\n", pt.control.analog_channels[0]);
        } else {
            Serial.printf("Battery voltage 0: %f\n", pt.response.battery_voltage);
        }
        
        
    }




}

