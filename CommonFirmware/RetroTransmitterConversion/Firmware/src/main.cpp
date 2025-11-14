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


int last_ms = 0;
void loop() {


    if(millis() - last_ms >= 1000) {

        Serial.printf("Send\n");

        last_ms = millis();

        Packet pt = {};
        pt.control.analog_channels[0] = 67;
        ptype ptt = CONTROL_TYPE;
        rh->sendPacket(&pt, ptt, TONODEID, false);
    }


    Packet pt = {};
    ptype ptt = {};
    if(rh->checkForPackets(pt, ptt)) {
        //nothing for now.
        Serial.printf("Analog channel 0: %d\n", pt.control.analog_channels[0]);
        
    }




}

