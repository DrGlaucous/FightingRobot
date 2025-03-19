
#include <Arduino.h>

#include <esp_now.h>
#include <esp_wifi.h>
#include <Wifi.h>
#include <DShotRMT.h>

#include "configuration.h"
#include "TimerTick.h"
#include "Robot.h"

//custom MAC addresses
uint8_t sender_addr[] = {0x30, 0xAE, 0xA4, 0x07, 0x0D, 0x64};
uint8_t rec_addr[] = {0xA0, 0x0E, 0x04, 0x0F, 0xFD, 0x64};

DShotRMT* esc;

RobotHandler* robot;

void setup()
{
    Serial.begin(115200);

    //robot = new RobotHandler();

    esc = new DShotRMT();
    esc->begin(ESC_PIN, DSHOT300, ENABLE_BIDIRECTION, MOTOR_POLE_COUNT);

    pinMode(MOTOR_1_ENCA, INPUT);

    pinMode(MOTOR_SLEEP_PIN, OUTPUT); //HIGH == enable

    pinMode(MOTOR_1A_PIN, OUTPUT);
    pinMode(MOTOR_1B_PIN, OUTPUT);

    pinMode(MOTOR_2A_PIN, OUTPUT);
    pinMode(MOTOR_2B_PIN, OUTPUT);

    digitalWrite(MOTOR_1A_PIN, 1);
    digitalWrite(MOTOR_1B_PIN, 0);

    digitalWrite(MOTOR_2A_PIN, 0);
    digitalWrite(MOTOR_2B_PIN, 0);
}

uint32_t counter = 0;

void loop()
{
    //Serial.println("B");
    //robot->update();

    if(millis() > counter + 1000) {
        //MOTOR_1_ENCA
        uint8_t aa = digitalRead(MOTOR_1_ENCA);
        uint8_t bb = !digitalRead(MOTOR_SLEEP_PIN);

        Serial.printf("ENCA: %d MOTOR SLEEP: %d\n", aa, bb);
        counter = millis();

        digitalWrite(MOTOR_SLEEP_PIN, bb);

    }

    esc->send_dshot_value(DSHOT_THROTTLE_MIN);
    delay(1);

}





