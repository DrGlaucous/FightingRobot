#include <Arduino.h>
#include <math.h>
#include "configuration.h"
#include "PidControllerF.h"
#include "MotorController.h"


int rpm_to_ticks(int rpm) {
    return rpm * 200;
}


static void isr_0();
static MotorController* this_0;

static void isr_1();
static MotorController* this_1;


void isr_0() {
    this_0->isr_handler();
}
void isr_1() {
    this_1->isr_handler();
}


void MotorController::isr_handler() {
    


    bool CLKstate = digitalRead(enca_pin);
    bool DTstate = digitalRead(encb_pin);

    switch (encoder_state) {
        case 0:                         // Idle state, encoder not turning
            if (!CLKstate){             // Turn clockwise and CLK goes low first
                encoder_state = 1;
            } else if (!DTstate) {      // Turn anticlockwise and DT goes low first
                encoder_state = 4;
            }
            break;
        // Clockwise rotation
        case 1:                     
            if (!DTstate) {             // Continue clockwise and DT will go low after CLK
                encoder_state = 2;
            } 
            break;
        case 2:
            if (CLKstate) {             // Turn further and CLK will go high first
                encoder_state = 3;
            }
            break;
        case 3:
            if (CLKstate && DTstate) {  // Both CLK and DT now high as the encoder completes one step clockwise
                encoder_state = 0;
                ++motor_tick_ct;
            }
            break;
        // Anticlockwise rotation
        case 4:                         // As for clockwise but with CLK and DT reversed
            if (!CLKstate) {
                encoder_state = 5;
            }
            break;
        case 5:
            if (DTstate) {
                encoder_state = 6;
            }
            break;
        case 6:
            if (CLKstate && DTstate) {
                encoder_state = 0;
                --motor_tick_ct;
            }
            break; 
    }

}


MotorController::MotorController(
    int enca_pin,
    int encb_pin,
    int motor_a_pin,
    int motor_b_pin,
    IsrSlotNumber slot_num
    ) {
    
    this->enca_pin = enca_pin;
    this->encb_pin = encb_pin;
    this->motor_a_pin = motor_a_pin;
    this->motor_b_pin = motor_b_pin;

    this->has_began = false;
    
    this->slot_num = slot_num;

    
    motor_pid = new PIDController(
        ANALOG_MIN,
        ANALOG_MAX,
        -ANALOG_MAX,
        0.0012, //p
        0.00001, //i
        0.01 //d
    );

    mpid_f = PIDControllerF_construct(
        ANALOG_MIN,
        ANALOG_MAX,
        -ANALOG_MAX,
        0.0012, //p
        0.00001, //i
        0.01 //d
    );

    //set proper ISR
    switch(slot_num) {
        case IsrSlotZero: {
            this_0 = this;
            attachInterrupt(enca_pin, isr_0, CHANGE);
            attachInterrupt(encb_pin, isr_0, CHANGE);
            break;
        }
        case IsrSlotOne: {
            this_1 = this;
            attachInterrupt(enca_pin, isr_1, CHANGE);
            attachInterrupt(encb_pin, isr_1, CHANGE);
            break;
        }
    }

}





MotorController::~MotorController() {

    delete(motor_pid);
    PIDControllerF_destruct(mpid_f);

    detachInterrupt(enca_pin);
    detachInterrupt(encb_pin);

    // switch(slot_num) {
    //     case IsrSlotZero: {
    //         this_0 = this;
    //         detachInterrupt(enca_pin);
    //         detachInterrupt(encb_pin);
    //         break;
    //     }
    //     case IsrSlotOne: {
    //         this_1 = this;
    //         attachInterrupt(enca_pin, isr_0, CHANGE);
    //         attachInterrupt(encb_pin, isr_0, CHANGE);
    //         break;
    //     }
    // }
}

void MotorController::begin() {
    //set up IO
    pinMode(motor_a_pin, OUTPUT);
    pinMode(motor_b_pin, OUTPUT);
    pinMode(enca_pin, INPUT);
    pinMode(encb_pin, INPUT);

    analogWrite(motor_a_pin, ANALOG_MIN);
    analogWrite(motor_b_pin, ANALOG_MIN);



    has_began = true;

}

//target speed should be an RPM, if in direct mode, it is clamped between -ANALOG_MAX and ANALOG_MAX (255)
void MotorController::tick(int target_speed, bool is_disabled, bool direct_speed_map) {

    //inputs
    //int target_speed = 0; //raw controller speed (from NORMAL_MIN to NORMAL_MAX)
    //bool is_disabled = false; //true when the main robot disables the motor controller

    //true when the input speed should be mapped directly to the motors
    //can be used if the PID breaks or something
    //bool direct_speed_map = false;



    //must have initialized and be enabled
    if(!has_began || is_disabled) {
        return;
    }


    //turn raw controller input into a target RPM speed for the motor
    //note: 200 ticks per rotation

    //get current RPM (may be less than one per tick) (note: does not update when motor is stalled!!)
    uint32_t microseconds = micros();
    uint32_t delta_micros = microseconds - last_micros;
    if(motor_tick_ct != last_motor_tick_ct || delta_micros > 100000)
    {
        // //store latest value
        // tick_history[tick_history_index] = motor_tick_ct - last_motor_tick_ct;
        // tick_history_index++;
        // tick_history_index %= HISTORY_COUNT;
        // //average all values
        // int history_sum = 0;
        // for(int i = 0; i < HISTORY_COUNT; ++i) {
        //     history_sum += tick_history[i];
        // }
        
        //convert to RPM
        float minutes_elapsed = ((float)(delta_micros) * 0.000001 * 0.016666);
        float rpm = (float)(motor_tick_ct - last_motor_tick_ct) / (minutes_elapsed * 200);

        last_motor_tick_ct = motor_tick_ct;
        last_micros = microseconds;
        current_rpm = rpm;
    }


    //only update speed on time change
    /*
    if(milliss != last_millis)
    {
        int delta_tickss = motor_tick_ct - last_motor_tick_ct;

        if(delta_tickss) {
            time_since_motor_tick = 0;
            dta_ticks = delta_tickss;
        } else {
            time_since_motor_tick++;
            dta_ticks = dta_ticks / ((float)time_since_motor_tick);
        }

        //tick_fraction is the number of ticks we have since the last time checked

        float minutes_elapsed = ((float)(milliss - last_millis) * 0.001 * 0.016666);
        float rpm = dta_ticks / minutes_elapsed;

        current_rpm = rpm;
        last_motor_tick_ct = motor_tick_ct;
    }
    */





    //run PID or not
    if(direct_speed_map) {
        if(target_speed > ANALOG_MAX) {
            target_speed = ANALOG_MAX;
        } else if(target_speed < -ANALOG_MAX) {
            target_speed = -ANALOG_MAX;
        }
        motor_output = target_speed;
    } else {
        


        //I'm not sure which one of these I want to use...

        //auto fixed_pid_out = PIDControllerF_tick(mpid_f, INT_TO_FIXED32(target_speed), FLOAT_TO_FIXED32(current_rpm), 200);

        auto float_pid_out = motor_pid->tick(target_speed, current_rpm, 200);

        motor_output = float_pid_out;
        
        //Serial.printf("RPM: %8.3f || Target: %d || FIXED: %5d\n", current_rpm, target_speed, fixed_pid_out);

    }

    //Serial.printf("RPM: %8.3f\n", current_rpm);


    write_motors();
    
    return;

}



void MotorController::write_motors() {

    analogWrite(motor_a_pin, motor_output < 0? 0: motor_output);
    analogWrite(motor_b_pin, motor_output < 0? -motor_output: 0);

}



