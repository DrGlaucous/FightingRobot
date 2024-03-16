#include <Arduino.h>
#include <math.h>

#ifdef USING_ESP32
#include <ESP32Servo.h>
#include <DShotRMT.h>
#endif

#include "Controller.h"
#include "ESPRadio.h"
#include "configuration.h"
#include "Robot.h"


#ifndef IS_CONTROLLER



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

    servo_1 = new Servo();
    servo_2 = new Servo();

    //moved to resume()
    //servo_1->attach(SERVO_1_PIN);
    //servo_2->attach(SERVO_2_PIN);




    radio = new RadioNowHandler();
    //blinker = new BlinkerHandler(LED_BUILTIN);
    mr_trig = new FastTrig();



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

    pause();

}


RobotHandler::~RobotHandler()
{
    delete(radio);
    //delete(blinker);
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

    //does basic IO processing, good for testing
    //TestMain();



    if(radio->CheckForPacket(NULL) == RX_SUCCESS)
    {
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


    servo_1->detach();
    servo_2->detach();


    //disable wheelbase
    digitalWrite(MOTOR_SLEEP_PIN, LOW);

    //no need to disable ESC: simply not writing to it will disable it (esp32)

    Serial.println("Paused!");

    //just in case a re-enable signal comes in, (or the timer overflows), the robot will not lurch suddenly
    xm = 0;
    ym = 0;
    esc_set_speed = ESC_SPEED_MIN_R;

}


void RobotHandler::resume()
{
    if(is_enabled)
        return;

    is_enabled = true;


    servo_1->attach(SERVO_1_PIN);
    servo_2->attach(SERVO_2_PIN);


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


    xm = map(gotten_data.analog_channels[X_IN], NORMAL_MIN, NORMAL_MAX, -XY_RADIUS, XY_RADIUS);
    ym = map(gotten_data.analog_channels[Y_IN], NORMAL_MIN, NORMAL_MAX, -XY_RADIUS, XY_RADIUS);

    //do non-linear rotation mapping
    rot_m = map(gotten_data.analog_channels[TURN_IN], NORMAL_MIN, NORMAL_MAX, XY_RADIUS, -XY_RADIUS);

    float degrees = mapf(gotten_data.analog_channels[TURN_RAMP_TUNE], NORMAL_MIN, NORMAL_MAX, 1.0, 5.0);
    rot_m = (int16_t)TurnMap(rot_m, XY_RADIUS, XY_RADIUS, degrees, 0);


    //we may want to use exponential mapping for this instead to get the finer values
    ramp_tune = mapf(gotten_data.analog_channels[MOTOR_RAMP_TUNE], NORMAL_MIN, NORMAL_MAX, 0.0, 3.0);

    is_flipped_over = gotten_data.digital_channels[FLIPOVER_IN] < 1;
    is_two_wheeled = gotten_data.digital_channels[TWO_MODE_IN] < 1;
    broken_wheel = gotten_data.digital_channels[TWO_SELECT_IN];
    remote_disable = gotten_data.digital_channels[MASTER_ENABLE] == 2;

    esc_reversed = gotten_data.digital_channels[ESC_REVERSE_IN];

    //from the knobs, controls how far the servo arm will rotoate with the stick
    servo_angle_min = map(gotten_data.analog_channels[SERVO_MIN_IN], NORMAL_MIN, NORMAL_MAX, SERVO_MIN, SERVO_MAX);

    //reverse mapping (for servo) happens later (because both servos take different values)
    servo_angle = map(gotten_data.analog_channels[SERVO_IN], NORMAL_MIN, NORMAL_MAX, servo_angle_min, SERVO_MAX);


    //handles reverse mapping (for ESC)
    esc_set_speed = esc_reversed == 1 ? DSHOT_CMD_MOTOR_STOP : 
    (!!is_flipped_over ^ !!esc_reversed) ? 
    map(gotten_data.analog_channels[ESC_IN], NORMAL_MAX, NORMAL_MIN, ESC_SPEED_MIN_R, ESC_SPEED_MAX_R) :
    map(gotten_data.analog_channels[ESC_IN], NORMAL_MAX, NORMAL_MIN, ESC_SPEED_MIN_F, ESC_SPEED_MAX_F);


}


void RobotHandler::SetWheelSpeedProportions()
{
    //Serial.printf("XM: %d\tYM %d\n", xm, ym);
    //xm = 0;
    //ym = XY_RADIUS;


    //rot_m = 0;

    //vector magnitude
    unsigned short magnitiude = (unsigned short)sqrt(xm*xm + ym*ym);
    if(magnitiude > XY_RADIUS)
        magnitiude = XY_RADIUS;

    //Serial.println(magnitiude);
    
    //vector direction
	unsigned char angle = mr_trig->GetArctan(xm, ym);

    //fast trig radius is 512
    //funny I'm using fast trig here when all my mapping functions use process-heavy FPL...
	
    //bottom
    mot1 = map(mr_trig->GetCos(angle), 0, 512, 0, magnitiude) + rot_m;
    //right
	mot2 = (map(mr_trig->GetCos(angle + 512 / 3), 0, 512, 0, magnitiude) + rot_m); //values are not exact, but are close enough in terms of integer offset
	//left
    mot3 = (map(mr_trig->GetCos(angle + 256 / 3), 0, 512, 0, magnitiude) + rot_m);

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


    //do logarithmic mapping
    mot1 = (int16_t)LogMap(mot1, XY_RADIUS, XY_RADIUS, ramp_tune);
    mot2 = (int16_t)LogMap(mot2, XY_RADIUS, XY_RADIUS, ramp_tune);
    mot3 = (int16_t)LogMap(mot3, XY_RADIUS, XY_RADIUS, ramp_tune);


    //Serial.printf("Mot1: %-4d Mot2: %-4d Mot3: %-4d\n", mot1, mot2, mot3);

    //flip motors based on reversed case:
    if(is_flipped_over)
    {
        int16_t mot_hold = mot2;
        mot2 = mot3;
        mot3 = mot_hold;
    }


	//traditional trig method:
	//*wheel_1 = cos(angle);
    //*wheel_2 = cos(angle + 4 * PI / 3.0);
    //*wheel_3 = cos(angle + 2 * PI / 3.0);

}


void RobotHandler::WriteMotors()
{
    //write drivebase
    analogWrite(MOTOR_1A_PIN, mot1 < 0? 0: mot1);
    analogWrite(MOTOR_1B_PIN, mot1 < 0? -mot1: 0);

    analogWrite(MOTOR_2A_PIN, mot2 < 0? 0: mot2);
    analogWrite(MOTOR_2B_PIN, mot2 < 0? -mot2: 0);

    analogWrite(MOTOR_3A_PIN, mot3 < 0? 0: mot3);
    analogWrite(MOTOR_3B_PIN, mot3 < 0? -mot3: 0);




    //write servos (using degrees)
    auto mirrored_servo_angle = SERVO_MAX == servo_angle_min ? servo_angle_min : map(servo_angle, servo_angle_min, SERVO_MAX, SERVO_MIN, servo_angle_min);
    servo_1->write(is_flipped_over? mirrored_servo_angle : servo_angle);
    servo_2->write(is_flipped_over? servo_angle : mirrored_servo_angle);

    //Serial.printf("S1: %5d S2: %5d\n", servo_angle, mirrored_servo_angle);

    //write ESC
    if(TimerHandler::DeltaTimeMillis(&last_esc_time, 2))
    {
        esc->send_dshot_value(esc_set_speed);
        auto error_t = esc->get_dshot_packet(&esc_get_speed);
    }


}


void RobotHandler::ReadVoltage()
{
    battery_voltage = analogRead(VOLTMETER_PIN) * VOLTMETER_SLOPE;
}


void RobotHandler::SendTelemetry()
{

    //prepare potenial response with latest data
    if(TimerHandler::DeltaTimeMillis(&last_time, 100))
    {
        ReadVoltage();
        remote_ack_packet_t outbox = {};
        outbox.battery_voltage = battery_voltage;
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


//turning stick equation (we may have been able to use a tanget equation for this one... Oh, well, the exponential should work just fine)
//map from, map to, degrees, slope
float RobotHandler::TurnMap(float input, float t, float u, float degrees, float linear_slope) {

	//float input = {};
	//float t,u = {}; //map from (t), to (u)
	//float linear_slope = {}; //V
	//float degrees = {}; //W


	//undefined prevention
	if (t == 0)
		return u; //y

	//we expect this to be symmetrical, if we want 2nd or 4th quad, we use negative u
	t = fabs(t);
	

	float absin = fabs(input);
	float a = (u / (pow(t, degrees)) * (1 - linear_slope));
	float c = linear_slope * (u / t);
	float result = a * pow(absin, degrees) + c * absin;


	//use negative half of the equation
	if (input < 0)
		result *= -1;

	return result;

}

//motor ramping equations
//map from, map to, curve strength (smaller is steeper), any value over 4 is pretty much a linear line
float RobotHandler::LogMap(float input, float i, float h, float strength) {

    //float input = {};
    //float strength; //infinity is a straight line, 0 is a step function, in between is a logarithmic ramp
    //float h, i = {}; //map from (h) to (i)

    //do it piecewise
    if(strength == 0.0)
        return input < 0.0 ? -i : i;

    //otherwise, do the tan, man!
    float f = strength * h;
    float a = (2.0*(h + f))/PI;
    float b = tan(h/a)/i;
    return a * atan(b*input);



}



#endif

