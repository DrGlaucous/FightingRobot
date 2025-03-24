#include <Arduino.h>
#include <math.h>


#include <ESP32Servo.h>
#include <DShotRMT.h>

#include "Controller.h"
#include "ESPRadio.h"
#include "configuration.h"
#include "MotorController.h"
#include "Robot.h"

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
    esc = new DShotRMT(ESC_PIN);
    esc->begin(DSHOT300, ENABLE_BIDIRECTION, MOTOR_POLE_COUNT);


    //moved to resume()
    //servo_1->attach(SERVO_1_PIN);
    //servo_2->attach(SERVO_2_PIN);




    radio = new RadioNowHandler();
    //blinker = new BlinkerHandler(LED_BUILTIN);




    //for motors
    motor_r = new MotorController(
        MOTOR_1_ENCA,
        MOTOR_1_ENCB,
        MOTOR_1A_PIN,
        MOTOR_1B_PIN,
        IsrSlotZero
    );
    motor_l = new MotorController(
        MOTOR_2_ENCB,
        MOTOR_2_ENCA,
        MOTOR_2B_PIN,
        MOTOR_2A_PIN,
        IsrSlotOne
    );

    motor_r->begin();
    motor_l->begin();

    //motor sleep mode
    pinMode(MOTOR_SLEEP_PIN, OUTPUT);
    
    //start with robot disabled
    is_enabled = false;


}


RobotHandler::~RobotHandler()
{
    delete(radio);
    delete(esc);

    delete(motor_r);
    delete(motor_l);

}


void RobotHandler::update()
{



    if(radio->CheckForPacket(NULL) == RX_SUCCESS)
    {
        //Serial.println("Got packet");

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
        digitalWrite(MOTOR_SLEEP_PIN, LOW); //HIGH == enable
        return;
    } else {
        digitalWrite(MOTOR_SLEEP_PIN, HIGH); //HIGH == enable

        motor_l->tick(motor_left_speed, false, direct_map_motors);
        motor_r->tick(motor_right_speed, false, direct_map_motors);


        //write ESC (we could also do this in an ISR, but...)
        auto delta_millis = millis() - last_esc_time;
        if(delta_millis > 2)
        {
            esc->send_dshot_value(esc_speed);
            auto error_t = esc->get_dshot_packet(&esc_rpm_telem);

            last_esc_time = millis();
        }

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

    //set ESC speed
    esc_speed = map(packet.channels.analog_channels[VERTICAL_LEFT], NORMAL_MAX, NORMAL_MIN, ESC_SPEED_MIN_R, ESC_SPEED_MAX_R);
    //disable for now
    //esc_speed = DSHOT_THROTTLE_MIN;

    //determine motor PID enabled
    direct_map_motors = !!packet.channels.digital_channels[6];

    //determine enabled state
    is_enabled = packet.channels.digital_channels[5];


    //change map range bsed on mode
    auto motor_speed_min = direct_map_motors? -ANALOG_MAX : -MOTOR_RPM_MAX;
    auto motor_speed_max = direct_map_motors? ANALOG_MAX : MOTOR_RPM_MAX;

    //get and normalize the values between our map range
    auto turn_value = map(packet.channels.analog_channels[HORIZONTAL_RIGHT], NORMAL_MAX, NORMAL_MIN, motor_speed_min, motor_speed_max);
    auto straight_value = map(packet.channels.analog_channels[VERTICAL_RIGHT], NORMAL_MAX, NORMAL_MIN, motor_speed_min, motor_speed_max);

    //forward values for each motor (crop values less than 5 for stability)
    auto motor_l_forward = abs(straight_value) < 5 ? 0 : straight_value;
    auto motor_r_forward = abs(straight_value) < 5 ? 0 : straight_value;



    //turn value is from 0 to 512, we will forward-map R and reverse map L
    auto motor_l_turn = map(turn_value, motor_speed_min, motor_speed_max, motor_speed_max, motor_speed_min);
    auto motor_r_turn = turn_value;

    
    // auto mlf = abs(motor_l_forward);
    // auto mlt = abs(motor_l_turn);
    // motor_left_speed = (mlf > mlt) ? motor_l_forward : motor_l_turn;

    // auto mrf = abs(motor_r_forward);
    // auto mrt = abs(motor_r_turn);
    // motor_right_speed = (mrf > mrt) ? motor_r_forward : motor_r_turn;
    // Serial.printf("MLF: %d, MLT: %d FINAL: %d|| MRF: %d, MRT: %d FINAL: %d\n", mlf, mlt, motor_left_speed, mrf, mrt, motor_right_speed);


    motor_left_speed = clamp(motor_l_forward + motor_l_turn, motor_speed_min, motor_speed_max);
    motor_right_speed = clamp(motor_r_forward + motor_r_turn, motor_speed_min, motor_speed_max);


}


void RobotHandler::SendTelemetry()
{

    //prepare potenial response with latest data
    if(TimerHandler::DeltaTimeMillis(&last_time, 100))
    {
        remote_ack_packet_t outbox = {};
        outbox.battery_voltage = 0.0;
        outbox.motor_rpm = esc_rpm_telem;
        radio->SendPacket(outbox);
    }

}


