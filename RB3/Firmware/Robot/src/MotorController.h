#pragma once

#include <Arduino.h>
#include "PidControllerF.h"
#include "PidController.h"

#define ANALOG_MAX 255
#define ANALOG_MIN 0

#define HISTORY_COUNT 64

typedef enum IsrSlotNumber {
    IsrSlotZero,
    IsrSlotOne
}IsrSlotNumber;

typedef struct OutPacket {
    float rpm;
    int motor_tick_ct;
    int motor_output;
}OutPacket;

class MotorController{
    

    public:

    MotorController(
        int enca_pin,
        int encb_pin,
        int motor_a_pin,
        int motor_b_pin,
        IsrSlotNumber sl
    );


    ~MotorController();

    void begin();

    void tick(int target_speed, bool is_disabled, bool direct_speed_map);

    void isr_handler();

    private:

    void write_motors();

    int enca_pin = -1;
    int encb_pin = -1;
    int motor_a_pin = -1;
    int motor_b_pin = -1;


    int motor_output = 0; //value to be written to the motor (result of the PID or direct mapping)
    bool has_began = false;

    int motor_tick_ct = 0; //holds raw absolute position of the motor
    int last_motor_tick_ct = 0; //for delta ticks
    int time_since_motor_tick = 0; //holds the number of cycles since the last encoder change interrupt

    float dta_ticks = 0.0;
    float current_rpm = 0.0;


    uint8_t encoder_state = 0; //state machine for getting encoder number

    PIDController* motor_pid = NULL;
    IsrSlotNumber slot_num = IsrSlotOne; //keep track of this for removing ISR on destruct


    //hold HISTORY_COUNT worth of delta ticks
    int8_t tick_history[HISTORY_COUNT];
    int tick_history_index = 0;
    uint32_t last_micros = 0;


    // // //isrs (one for each instance of the class we can run)
    // // static void isr_0();
    // // static MotorController* this_0;

    // // static void isr_1();
    // // static MotorController* this_1;



};

















