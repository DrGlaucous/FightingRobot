#include <Arduino.h>
#include <SPI.h>


#include "configuration.h"
//#include "RFM69HW.h"


//RadioHandler radio;


RFM69* radio = nullptr; //new RFM69((uint8_t)PIN_NSS, (uint8_t)PIN_ISR, true, &SPI);


int incr = 0;
void onGet() {
    incr += 1;
}

void setup() {

    Serial.begin(115200);


    Serial.printf("Program start\n");

#ifdef USING_ESP32
    SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI);
#elif USING_STM32
    SPI.begin();
#endif

    Serial.printf("SPI started\n");


    ////////////////////////////

    radio = new RFM69((uint8_t)PIN_NSS, (uint8_t)PIN_ISR, true, &SPI);
    radio->initialize(FREQUENCY, MYNODEID, NETWORKID);
    radio->setHighPower();

#ifdef ENCRYPTKEY
    radio->encrypt(ENCRYPTKEY);
#endif


  Serial.print("Radio initialized");

}


bool flip = false;
int last_ms = 0;
void loop() {




    if(millis() - last_ms >= 1000) {
        flip = !flip;
        last_ms = millis();
        Serial.printf("Sending..\n");

        char sendBuffer2[] = {"Hello, there"};
        radio->send(TONODEID, sendBuffer2, sizeof(sendBuffer2));
    }

    //always be checking for packets
    if (radio->receiveDone())
    {
        Serial.print("received from node ");
        Serial.print(radio->SENDERID, DEC);
        Serial.print(", message [");

        for (byte i = 0; i < radio->DATALEN; i++)
              Serial.print((char)radio->DATA[i]);

        Serial.print("], RSSI ");
        Serial.println(radio->RSSI);

        if (radio->ACKRequested())
        {
            radio->sendACK();
            Serial.println("ACK sent");
        }
    }

    delay(1);

}

