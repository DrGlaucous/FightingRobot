#pragma once
#include <Arduino.h>
#include <ESP32Servo.h>
#include <DShotRMT.h>

#include "Controller.h"
#include "ESPRadio.h"
#include "configuration.h"
#include "TimerTick.h"






//does all the roboty things (like movement, weapon handling, etc.)
class RobotHandler
{

    public:

    RobotHandler();
    ~RobotHandler();

    void update();

    private:

    void MapControllerData();
    void SendTelemetry();

    //for sending out telemetry packets
    unsigned long last_time = 0;
    //used to update the ESC output (only in dshot mode)
    unsigned long last_esc_time = 0;


    RadioNowHandler* radio;


    //last packet read from the radio
    concatated_channels_t gotten_data = {};
    
    //wheelbase vectors left side and right side
    //should be a raw value from NORMAL_MIN to NORMAL_MAX, mapping is handled in the MotorController class
    int16_t motor_left_speed, motor_right_speed {};
    bool direct_map_motors = false;

    Servo servo = {};
    int16_t servo_angle = 0;

    //enables/disables weapon and drivetrain
    bool is_enabled = false;





    //test: dump channels to serial monitor
    void DumpChannelPacket();
    //dump processed cahnnels to serial moditor
    void DumpMappedPacket();

};



