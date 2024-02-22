#include <Arduino.h>

#ifdef USING_ESP32
#include <ESP32Servo.h>
#include <DShotRMT.h>
#elif USING_STM32_BP
#include <STM32_ISR_Servo.h>
#endif

#include "Controller.h"
#include "Radio.h"
#include "configuration.h"
#include "Robot.h"


#ifndef IS_CONTROLLER






RobotHandler::RobotHandler()
{
    auto startup_settings = radio_handler_config_datapack_t
    {
        SLAVE_PIN,
        IRQ_PIN,
        FREQUENCY,
        RECEIVERNODEID,
        NETWORKID,
        IS_HIGH_POWER,
        ENCRYPT,
        ENCRYPTKEY,
    };
    radio = new RadioHandler(startup_settings);
    blinker = new BlinkerHandler(LED_BUILTIN);
    mr_trig = new FastTrig();

#ifdef USING_ESP32
    //init esc, note: we assume the ESC is in 3D mode
    esc = new DShotRMT(ESC_PIN);
    esc->begin(DSHOT600, ENABLE_BIDIRECTION, MOTOR_POLE_COUNT);

    servo_1 = new Servo();
    servo_2 = new Servo();

    //moved to resume()
    //servo_1->attach(SERVO_1_PIN);
    //servo_2->attach(SERVO_2_PIN);

#elif USING_STM32_BP
    //todo: add this
#endif

    //for voltmeter
    pinMode(VOLTMETER_PIN, INPUT);

    //for motors
    pinMode(MOTOR_1A_PIN, OUTPUT);
    pinMode(MOTOR_1B_PIN, OUTPUT);

    pinMode(MOTOR_2A_PIN, OUTPUT);
    pinMode(MOTOR_2B_PIN, OUTPUT);

    pinMode(MOTOR_3A_PIN, OUTPUT);
    pinMode(MOTOR_3B_PIN, OUTPUT);

    pinMode(MOTOR_SLEEP_PIN, OUTPUT);


    resume();

}


RobotHandler::~RobotHandler()
{
    delete(radio);
    delete(blinker);
    delete(mr_trig);
}


void RobotHandler::update()
{

    //pulse test
    // blinker->update();
    // if(gTimer.DeltaTimeMillis(&last_time, 1000))
    // {
    //     blinker->blink_lt(100);
    // }



    //prepare potenial response with latest data
    remote_ack_packet_t ackpt = {};
    ackpt.motor_rpm = esc_speed;
    ackpt.battery_voltage = battery_voltage;

    //sniff for packets (and send ack if we got any)
    response_status_t responsee = radio->CheckForResponse(NULL, ackpt);
    //perform action based on response type
    if(responsee == RX_SENT_ACK || responsee == RX_SUCCESS)
        DumpChannelPacket();


    //take rx packets and format them for each application
    MapControllerData();

    //do the omniwheel math
    SetWheelSpeedProportions();
    
    //analogWrite to each motor driver
    WriteMotors();



}


void RobotHandler::pause()
{
    servo_1->detach();
    servo_2->detach();

    //disable wheelbase
    digitalWrite(MOTOR_SLEEP_PIN, LOW);

    //no need to disable ESC: simply not writing to it will disable it (esp32)

}


void RobotHandler::resume()
{
    servo_1->attach(SERVO_1_PIN);
    servo_2->attach(SERVO_2_PIN);

    //enable wheelbase
    digitalWrite(MOTOR_SLEEP_PIN, HIGH);

}


void RobotHandler::DumpChannelPacket()
{
    blinker->blink_lt(100);
    

    //dump recieved channels
    for(int i = 0; i < ANALOG_CHANNEL_CNT; ++i)
    {
        Serial.printf("%d : %d||", i, radio->GetLastControlPacket().channels.analog_channels[i]);
    }
    Serial.printf("|//|");
    for(int i = 0; i < DIGITAL_CHANNEL_CNT * 2; ++i)
    {
        Serial.printf("%d : %d||", i, radio->GetLastControlPacket().channels.digital_channels[i]);
    }
    Serial.printf("\n");
}


void RobotHandler::MapControllerData()
{
    auto gotten_data = radio->GetLastControlPacket().channels;

    xm = map(gotten_data.analog_channels[X_IN], NORMAL_MIN, NORMAL_MAX, -XY_RADIUS, XY_RADIUS);
    ym = map(gotten_data.analog_channels[Y_IN], NORMAL_MIN, NORMAL_MAX, -XY_RADIUS, XY_RADIUS);
    rot_m = map(gotten_data.analog_channels[TURN_IN], NORMAL_MIN, NORMAL_MAX, -XY_RADIUS, XY_RADIUS);


    is_flipped_over = gotten_data.analog_channels[FLIPOVER_IN] < 1;
    is_two_wheeled = gotten_data.analog_channels[TWO_MODE_IN] < 1;
    broken_wheel = gotten_data.analog_channels[TWO_SELECT_IN];
    esc_reversed = gotten_data.analog_channels[ESC_REVERSE_IN];

    //from the knobs, controls how far the servo arm will rotoate with the stick
    servo_angle_min = map(gotten_data.analog_channels[SERVO_MIN_IN], NORMAL_MIN, NORMAL_MAX, SERVO_MIN, SERVO_MAX);
    servo_angle_max = map(gotten_data.analog_channels[SERVO_MAX_IN], NORMAL_MIN, NORMAL_MAX, SERVO_MIN, SERVO_MAX);

    //reverse mapping (for servo) happens later (because both servos take different values)
    servo_angle = map(gotten_data.analog_channels[SERVO_IN], NORMAL_MIN, NORMAL_MAX, servo_angle_min, servo_angle_max);

    //handles reverse mapping (for ESC)
    esc_speed = map(gotten_data.analog_channels[ESC_IN], NORMAL_MIN, NORMAL_MAX, ESC_SPEED_MIDDLE, (is_flipped_over ^ esc_reversed) ? ESC_SPEED_MAX : ESC_SPEED_MIN);





}


void RobotHandler::SetWheelSpeedProportions()
{

    //vector magnitude
    unsigned short magnitiude = (unsigned short)sqrt(xm*xm + ym*ym);
    if(magnitiude > XY_RADIUS)
        magnitiude = XY_RADIUS;
    
    //vector direction
	unsigned char angle = mr_trig->GetArctan(xm, ym);

    //fast trig radius is 512
	mot1 = map(mr_trig->GetCos(angle), 0, 512, 0, XY_RADIUS) + rot_m;
	mot2 = map(mr_trig->GetCos(angle + 512 / 3), 0, 512, 0, XY_RADIUS) + rot_m; //values are not exact, but are close enough in terms of integer offset
	mot3 = map(mr_trig->GetCos(angle + 256 / 3), 0, 512, 0, XY_RADIUS) + rot_m;

    //probably a more efficient way to do this...

    //speed limiting
    if(mot1 > XY_RADIUS)
        mot1 = XY_RADIUS;
    if(mot1 < -XY_RADIUS)
        mot1 = -XY_RADIUS;

    if(mot2 > XY_RADIUS)
        mot2 = XY_RADIUS;
    if(mot2 < -XY_RADIUS)
        mot2 = -XY_RADIUS;

    if(mot3 > XY_RADIUS)
        mot3 = XY_RADIUS;
    if(mot3 < -XY_RADIUS)
        mot3 = -XY_RADIUS;

    //flip motors based on reversed case:
    //todo: this


	//traditional trig method:
	//*wheel_1 = cos(angle);
    //*wheel_2 = cos(angle + 4 * PI / 3.0);
    //*wheel_3 = cos(angle + 2 * PI / 3.0);

}


void RobotHandler::WriteMotors()
{
    //write drivebase
    analogWrite(MOTOR_1A_PIN, mot1 < 0? 0: mot1);
    analogWrite(MOTOR_1B_PIN, mot1 > 0? mot1: 0);

    analogWrite(MOTOR_2A_PIN, mot2 < 0? 0: mot2);
    analogWrite(MOTOR_2B_PIN, mot2 > 0? mot2: 0);

    analogWrite(MOTOR_3A_PIN, mot3 < 0? 0: mot3);
    analogWrite(MOTOR_3B_PIN, mot3 > 0? mot3: 0);

    //write servos (using degrees)
    auto mirrored_servo_angle = map(servo_angle, servo_angle_min, servo_angle_max, servo_angle_max, servo_angle_min);
    servo_1->write(is_flipped_over? mirrored_servo_angle : servo_angle);
    servo_2->write(is_flipped_over? servo_angle : mirrored_servo_angle);



    //write ESC
#ifdef USING_ESP32
    if(gTimer.DeltaTimeMillis(&last_esc_time, 2))
    {
        esc->send_dshot_value(esc_speed);
        auto error_t = esc->get_dshot_packet(&esc_speed);
    }
#elif USING_STM32_BP
    //todo: add this
#endif


}


void RobotHandler::ReadVoltage()
{
    battery_voltage = analogRead(VOLTMETER_PIN) * VOLTMETER_SLOPE;
}




#endif

