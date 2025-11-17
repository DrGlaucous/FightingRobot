#include <Arduino.h>
#include <math.h>


#include <ESP32Servo.h>
#include <DShotRMT.h>

#include "Controller.h"
#include "ESPRadio.h"
#include "configuration.h"
#include "Robot.h"


#define ANALOG_MAX 255
#define ANALOG_MIN 0


//motor is shorted, braking enabled
//HIGH-HIGH
//LOW-LOW

//direction 1
//HIGH-LOW

//direction 2
//LOW-HIGH



int clamp(int in, int min, int max) {
    if(in > max) {
        in = max;
    } else if(in < min) {
        in = min;
    }
    return in;
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max) {
    const float run = in_max - in_min;
    if(run == 0.0){
        log_e("map(): Invalid input range, min == max");
        return -1; // AVR returns -1, SAM returns 0
    }
    const float rise = out_max - out_min;
    const float delta = x - in_min;
    return (delta * rise) / run + out_min;
}


RobotHandler::RobotHandler()
{

    //init esc, note: we assume the ESC is in 3D mode
    servo.attach(ESC_PIN);


    radio = new RadioNowHandler();


    //for motors
    pinMode(MOTOR_1A_PIN, OUTPUT);
    pinMode(MOTOR_1B_PIN, OUTPUT);
    pinMode(MOTOR_2A_PIN, OUTPUT);
    pinMode(MOTOR_2B_PIN, OUTPUT);

    pinMode(SLEEP_PIN, OUTPUT);
    digitalWrite(SLEEP_PIN, 1);

    
    //start with robot disabled
    is_enabled = false;


}


RobotHandler::~RobotHandler()
{
    delete(radio);
}


void RobotHandler::update()
{



    if(radio->CheckForPacket(NULL) == RX_SUCCESS)
    {
        Serial.printf("Got");
        //take rx packets and format them for internal variables
        MapControllerData();
    }
    else if(radio->GetDeltaTime() > KEEPALIVE_TIMEOUT_MS)
    {
        //signal loss estop
        is_enabled = false;
    }



    //send back information about the robot on regular intervals
    SendTelemetry();










    //estop enabled, don't execute any physical functions
    if(!is_enabled) {
    
        if(servo.attached()) {
            servo.detach();
        }

        analogWrite(MOTOR_1A_PIN, 0);
        analogWrite(MOTOR_1B_PIN, 0);
        analogWrite(MOTOR_2A_PIN, 0);
        analogWrite(MOTOR_2B_PIN, 0);
        return;


    } else {

        if(!servo.attached()) {
            servo.attach(ESC_PIN);
        }

        servo.write(servo_angle);
        

        analogWrite(MOTOR_1A_PIN, motor_left_speed < 0? 0: motor_left_speed);
        analogWrite(MOTOR_1B_PIN, motor_left_speed < 0? -motor_left_speed: 0);

        analogWrite(MOTOR_2A_PIN, motor_right_speed < 0? 0: motor_right_speed);
        analogWrite(MOTOR_2B_PIN, motor_right_speed < 0? -motor_right_speed: 0);


    }


    //DumpChannelPacket();

}




void RobotHandler::DumpChannelPacket()
{
    //dump recieved channels
    for(int i = 0; i < ANALOG_CHANNEL_CNT; ++i)
    {
        Serial.printf(" %d :%4d ||", i, radio->GetLastControlPacket().channels.analog_channels[i]);
    }
    Serial.printf("|//|");
    for(int i = 0; i < DIGITAL_CHANNEL_CNT * 2; ++i)
    {
        Serial.printf("%d :%2d ||", i, radio->GetLastControlPacket().channels.digital_channels[i]);
    }
    Serial.printf("\n");
}

void RobotHandler::DumpMappedPacket()
{
    //dump recieved channels
    //Serial.printf("XM: %5d YM: %5d ROT: %5d\n", xm, ym, rot_m);
    //Serial.printf("M1: %5d M2: %5d M3: %5d\n", mot1, mot2, mot3);

}

void RobotHandler::MapControllerData() {

    //get most recent RX packet
    auto packet = radio->GetLastControlPacket();

    //get ESC speed
    servo_angle = map(packet.channels.analog_channels[VERTICAL_LEFT], NORMAL_MAX, NORMAL_MIN, 0, 180);


    //determine enabled state
    is_enabled = packet.channels.digital_channels[5];


    //change map range based on mode
    auto motor_speed_min = -ANALOG_MAX;
    auto motor_speed_max = ANALOG_MAX;

    //get and normalize the values between our map range
    auto turn_value = map(packet.channels.analog_channels[HORIZONTAL_RIGHT], NORMAL_MAX, NORMAL_MIN, motor_speed_min, motor_speed_max);
    auto straight_value = map(packet.channels.analog_channels[VERTICAL_RIGHT], NORMAL_MAX, NORMAL_MIN, motor_speed_min, motor_speed_max);

    turn_value = (abs(turn_value) < 5) ? 0 : turn_value;
    straight_value = (abs(straight_value) < 5) ? 0 : straight_value;

    //forward values for each motor (crop values less than 5 for stability)
    auto motor_l_forward = straight_value;
    auto motor_r_forward = straight_value;



    //turn value is from 0 to 512, we will forward-map R and reverse map L
    auto motor_l_turn = map(turn_value, motor_speed_min, motor_speed_max, motor_speed_max, motor_speed_min);
    auto motor_r_turn = turn_value;

    
    //debug dump
    // auto mlf = abs(motor_l_forward);
    // auto mlt = abs(motor_l_turn);
    // motor_left_speed = (mlf > mlt) ? motor_l_forward : motor_l_turn;
    // auto mrf = abs(motor_r_forward);
    // auto mrt = abs(motor_r_turn);
    // motor_right_speed = (mrf > mrt) ? motor_r_forward : motor_r_turn;
    // Serial.printf("MLF: %d, MLT: %d FINAL: %d|| MRF: %d, MRT: %d FINAL: %d\n", mlf, mlt, motor_left_speed, mrf, mrt, motor_right_speed);


    //clamp motor speeds between analogWrite values
    motor_left_speed = clamp(motor_l_forward + motor_l_turn, motor_speed_min, motor_speed_max);
    motor_right_speed = clamp(motor_r_forward + motor_r_turn, motor_speed_min, motor_speed_max);


    Serial.printf("MLF: %d || MLR: %d || Servo: %d\n", motor_left_speed, motor_right_speed, servo_angle);

}


void RobotHandler::SendTelemetry()
{

    //prepare potenial response with latest data
    if(TimerHandler::DeltaTimeMillis(&last_time, 100))
    {
        remote_ack_packet_t outbox = {};
        outbox.battery_voltage = 0.0;
        outbox.motor_rpm = 0.0;
        radio->SendPacket(outbox);
    }

}


