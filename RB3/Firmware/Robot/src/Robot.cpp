#include <Arduino.h>
#include <math.h>


#include <ESP32Servo.h>
#include <DShotRMT.h>

#include "Controller.h"
#include "ESPRadio.h"
#include "configuration.h"
#include "Robot.h"



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
    pinMode(MOTOR_1A_PIN, OUTPUT);
    pinMode(MOTOR_1B_PIN, OUTPUT);

    pinMode(MOTOR_2A_PIN, OUTPUT);
    pinMode(MOTOR_2B_PIN, OUTPUT);

    pinMode(MOTOR_SLEEP_PIN, OUTPUT);

    pause();

}


RobotHandler::~RobotHandler()
{
    delete(radio);
    //delete(blinker);
}


void RobotHandler::update()
{

    //pulse test
    // blinker->update();
    // if(gTimer.DeltaTimeMillis(&last_time, 1000))
    // {
    //     blinker->blink_lt(100);
    // }

    //does basic IO processing, good for testing
    //TestMain();



    if(radio->CheckForPacket(NULL) == RX_SUCCESS)
    {
        Serial.println("Got packet");

        //take rx packets and format them for each application
        MapControllerData();

        //check for remote halt
        if(remote_disable)
            pause();
        else
            resume();

    }
    else if(radio->GetDeltaTime() > KEEPALIVE_TIMEOUT_MS)
    {
        //signal loss estop
        pause();
    }


    //send back information about the robot on regular intervals
    SendTelemetry();

    //do the omniwheel math
    SetWheelSpeedProportions();


    //estop enabled, don't execute any physical functions
    if(!is_enabled)
        return;

    //analogWrite to each motor driver
    WriteMotors();

    //DumpChannelPacket();

}


void RobotHandler::pause()
{
    if(!is_enabled)
        return;
    
    is_enabled = false;


    //disable wheelbase
    digitalWrite(MOTOR_SLEEP_PIN, LOW);

    //no need to disable ESC: simply not writing to it will disable it (esp32)

    Serial.println("Paused!");

    //just in case a re-enable signal comes in, (or the timer overflows), the robot will not lurch suddenly


    esc_set_speed = ESC_SPEED_MIN_R;

}


void RobotHandler::resume()
{
    if(is_enabled)
        return;

    is_enabled = true;



    //enable wheelbase
    digitalWrite(MOTOR_SLEEP_PIN, HIGH);

    Serial.println("Resumed!");
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


void RobotHandler::MapControllerData()
{
    auto gotten_data = radio->GetLastControlPacket().channels;
    
    // if(gotten_data.analog_channels[TURN_IN] > 300 || gotten_data.analog_channels[TURN_IN] < 240)
    // {
    //     Serial.println(gotten_data.analog_channels[TURN_IN]);
    // }

    

    //gotten_data.digital_channels[FLIPOVER_IN]
    //gotten_data.analog_channels[ESC_IN]


    left = map(gotten_data.analog_channels[VERTICAL_LEFT], NORMAL_MIN, NORMAL_MAX, -MOTOR_RPM_MAX, MOTOR_RPM_MAX);
    right = map(gotten_data.analog_channels[HORIZONTAL_LEFT], NORMAL_MIN, NORMAL_MAX, -MOTOR_RPM_MAX, MOTOR_RPM_MAX);

    //do non-linear rotation mapping
    //rot_m = map(gotten_data.analog_channels[HORIZONTAL_RIGHT], NORMAL_MIN, NORMAL_MAX, MOTOR_RPM_MAX, -MOTOR_RPM_MAX);

    //float degrees = mapf(gotten_data.analog_channels[TUNER_RIGHT], NORMAL_MIN, NORMAL_MAX, 1.0, 5.0);

    //TEST
    //temp_servo2_min = map(gotten_data.analog_channels[TURN_RAMP_TUNE], NORMAL_MIN, NORMAL_MAX, SERVO_2_MS_MAX - 100, SERVO_2_MS_MAX + 100);
    //temp_servo2_max = map(gotten_data.analog_channels[MOTOR_RAMP_TUNE], NORMAL_MIN, NORMAL_MAX, SERVO_2_MS_MIN - 100, SERVO_2_MS_MIN + 100);



    //we may want to use exponential mapping for this instead to get the finer values
    //ramp_tune = mapf(gotten_data.analog_channels[MOTOR_RAMP_TUNE], NORMAL_MIN, NORMAL_MAX, 0.0, 3.0);

    //is_flipped_over = gotten_data.digital_channels[FLIPOVER_IN] < 1;
    //is_two_wheeled = gotten_data.digital_channels[TWO_MODE_IN] > 1;
    //broken_wheel = gotten_data.digital_channels[TWO_SELECT_IN];
    //remote_disable = gotten_data.digital_channels[MASTER_ENABLE] == 2;

    //esc_reversed = gotten_data.digital_channels[ESC_REVERSE_IN];

    //from the back right lever, controls how far the servo arm will rotoate with the stick (0-180)
    //servo_angle_min = map(gotten_data.analog_channels[SERVO_MIN_IN], NORMAL_MAX, NORMAL_MIN, SERVO_MIN, SERVO_MAX);

    //reverse mapping (for servo) happens later (because both servos take different values)
    //servo_angle = map(gotten_data.analog_channels[SERVO_IN], NORMAL_MAX, NORMAL_MIN, servo_angle_min, SERVO_MAX);


    //handles reverse mapping (for ESC)
    //esc_set_speed = esc_reversed == 1 ? DSHOT_CMD_MOTOR_STOP : 
    //(!!is_flipped_over ^ !!esc_reversed) ? 
    //map(gotten_data.analog_channels[VERTICAL_RIGHT], NORMAL_MAX, NORMAL_MIN, ESC_SPEED_MIN_F, ESC_SPEED_MAX_F):
    //map(gotten_data.analog_channels[VERTICAL_RIGHT], NORMAL_MAX, NORMAL_MIN, ESC_SPEED_MIN_R, ESC_SPEED_MAX_R);


}


void RobotHandler::SetWheelSpeedProportions()
{
    //Serial.printf("XM: %d\tYM %d\n", xm, ym);
    //xm = 0;
    //ym = XY_RADIUS;
    //rot_m = 0;

}


void RobotHandler::WriteMotors()
{
    //write drivebase
    analogWrite(MOTOR_1A_PIN, mot1 < 0? 0: mot1);
    analogWrite(MOTOR_1B_PIN, mot1 < 0? -mot1: 0);

    analogWrite(MOTOR_2A_PIN, mot2 < 0? 0: mot2);
    analogWrite(MOTOR_2B_PIN, mot2 < 0? -mot2: 0);

    //Serial.printf("left: %d\n", left);

    // //write servos (using degrees)
    // auto mirrored_servo_angle = SERVO_MAX - servo_angle;
    // int16_t demapped_1 = map(is_flipped_over? mirrored_servo_angle : servo_angle, SERVO_MIN, SERVO_MAX, SERVO_1_MS_MIN, SERVO_1_MS_MAX);
    // int16_t demapped_2 = map(is_flipped_over? servo_angle : mirrored_servo_angle, SERVO_MIN, SERVO_MAX, SERVO_2_MS_MIN, SERVO_2_MS_MAX);
    // servo_1->write(demapped_1);
    // servo_2->write(demapped_2);

    // //Serial.printf("S1: %d S2: %d\n", demapped_1, demapped_2);
    // //Serial.printf("S1: %5d S2: %5d || M2Min: %5d :: M2Max %5d\n", servo_1->readMicroseconds(), servo_2->readMicroseconds(), temp_servo2_min, temp_servo2_max);

    // //write ESC
    // if(TimerHandler::DeltaTimeMillis(&last_esc_time, 2))
    // {
    //     esc->send_dshot_value(esc_set_speed);
    //     uint32_t aa = 0;
    //     auto error_t = esc->get_dshot_packet(&aa);
    //     esc_get_speed = aa;
    // }


}


void RobotHandler::SendTelemetry()
{

    //prepare potenial response with latest data
    if(TimerHandler::DeltaTimeMillis(&last_time, 100))
    {
        remote_ack_packet_t outbox = {};
        outbox.battery_voltage = 0.0;
        outbox.motor_rpm = esc_get_speed;
        radio->SendPacket(outbox);
    }

}


void RobotHandler::TestMain()
{

    //send telemetry back on a regular interval
    SendTelemetry();

    if(radio->CheckForPacket(NULL) == RX_SUCCESS)
    {
        //auto commands = radio->GetLastControlPacket();
        DumpChannelPacket();
        //Serial.printf("%d || %d ||-|| %d || %d\n", commands.channels.analog_channels[2], commands.channels.analog_channels[3], commands.channels.digital_channels[0], commands.channels.digital_channels[1]);
    }
}
