
#include <Arduino.h>

#include <esp_now.h>
#include <esp_wifi.h>
#include <Wifi.h>
#include <DShotRMT.h>

#include "configuration.h"
#include "TimerTick.h"

#include "Robot.h"


#include <Adafruit_NeoPixel.h>

RobotHandler* robot = nullptr;

//esp32 supermini plus has a neopixel on pin 8.
//it shares this pin with a traditional LED (1 == ON)
//Adafruit_NeoPixel pixels(1, 8, NEO_GRB + NEO_KHZ800);

void setup()
{
    Serial.begin(115200);

    // pinMode(SLEEP_PIN, OUTPUT);
    // digitalWrite(SLEEP_PIN, 1);
    // pinMode(5, OUTPUT);
    // pinMode(6, OUTPUT);
    // digitalWrite(5, 0);
    // digitalWrite(6, 0);

    // pinMode(9, OUTPUT);
    // pinMode(7, OUTPUT);
    // digitalWrite(9, 0);

    // while(1) {
    //     digitalWrite(7, 0);
    //     digitalWrite(9, 1);
    //     delay(1000);
    //     digitalWrite(9, 0);
    //     digitalWrite(7, 1);
    //     delay(1000);
    // }


    robot = new RobotHandler();

}


void loop()
{
    robot->update();


}





