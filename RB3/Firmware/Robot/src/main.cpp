
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

DShotRMT* esc = nullptr;
RobotHandler* robot = nullptr;

//1 rotation == 200 pulses from the encoder
uint32_t last_encoder_value = 0;
uint32_t encoder_value = 0;

IRAM_ATTR void motor_isr() {
    //this function should never be executed outside the ITTR
    //encoder_value = rotary_encoder->read();

    static uint8_t state = 0;
    bool CLKstate = digitalRead(MOTOR_1_ENCA);
    bool DTstate = digitalRead(MOTOR_1_ENCB);

    switch (state) {
        case 0:                         // Idle state, encoder not turning
            if (!CLKstate){             // Turn clockwise and CLK goes low first
                state = 1;
            } else if (!DTstate) {      // Turn anticlockwise and DT goes low first
                state = 4;
            }
            break;
        // Clockwise rotation
        case 1:                     
            if (!DTstate) {             // Continue clockwise and DT will go low after CLK
                state = 2;
            } 
            break;
        case 2:
            if (CLKstate) {             // Turn further and CLK will go high first
                state = 3;
            }
            break;
        case 3:
            if (CLKstate && DTstate) {  // Both CLK and DT now high as the encoder completes one step clockwise
                state = 0;
                ++encoder_value;
            }
            break;
        // Anticlockwise rotation
        case 4:                         // As for clockwise but with CLK and DT reversed
            if (!CLKstate) {
                state = 5;
            }
            break;
        case 5:
            if (DTstate) {
                state = 6;
            }
            break;
        case 6:
            if (CLKstate && DTstate) {
                state = 0;
                --encoder_value;
            }
            break; 
    }

}


#include "Apple.h"

Apple* aa = nullptr;




void setup()
{
    Serial.begin(115200);

    //robot = new RobotHandler();

    //esc = new DShotRMT();
    //esc->begin(ESC_PIN, DSHOT300, ENABLE_BIDIRECTION, MOTOR_POLE_COUNT);

    pinMode(MOTOR_1_ENCA, INPUT);
    pinMode(MOTOR_1_ENCB, INPUT);

    pinMode(MOTOR_SLEEP_PIN, OUTPUT); //HIGH == enable
    digitalWrite(MOTOR_SLEEP_PIN, 1);

    pinMode(MOTOR_1A_PIN, OUTPUT);
    pinMode(MOTOR_1B_PIN, OUTPUT);

    pinMode(MOTOR_2A_PIN, OUTPUT);
    pinMode(MOTOR_2B_PIN, OUTPUT);

    digitalWrite(MOTOR_1A_PIN, 1);
    digitalWrite(MOTOR_1B_PIN, 0);

    digitalWrite(MOTOR_2A_PIN, 0);
    digitalWrite(MOTOR_2B_PIN, 0);

    // motor = new MotorController(
    //     MOTOR_1_ENCA,
    //     MOTOR_1_ENCB,
    //     MOTOR_1A_PIN,
    //     MOTOR_1B_PIN,
    //     IsrSlotZero
    // );

}

uint32_t counter = 0;
uint32_t last_millis = 0;

void loop()
{
    //Serial.println("B");
    //robot->update();

    // if(millis() > counter + 1000) {
    //     //MOTOR_1_ENCA
    //     uint8_t aa = digitalRead(MOTOR_1_ENCA);
    //     uint8_t bb = !digitalRead(MOTOR_SLEEP_PIN);

    //     Serial.printf("ENCA: %d MOTOR SLEEP: %d\n", aa, bb);
    //     counter = millis();

    //     digitalWrite(MOTOR_SLEEP_PIN, bb);

    // }

    // esc->send_dshot_value(DSHOT_THROTTLE_MIN);
    // delay(1);

    // auto rpm = motor->tick(
    //     256,
    //     false,
    //     true
    // );

    if(millis() != last_millis) {
        last_millis = millis();
        Serial.printf("RPM: %f\n", 0);
    }


}





