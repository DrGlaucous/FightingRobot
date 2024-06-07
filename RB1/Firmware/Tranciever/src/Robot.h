#pragma once
#include <Arduino.h>

#include "Controller.h"
#include "ESPRadio.h"
#include "configuration.h"
#include "TimerTick.h"

#ifdef USING_ESP32
#include <ESP32Servo.h>
#include <DShotRMT.h>
#endif


#ifndef IS_CONTROLLER



//test class for light blinking

class BlinkerHandler
{
    public:

    BlinkerHandler(int pin_no)
    {
        pinMode(pin_no, OUTPUT);
        lite_pin = pin_no;
    }

    
    ~BlinkerHandler()
    {

    }


    void update()
    {   
        if(TimerHandler::DeltaTimeMillis(&last_time, delta_time))
        {
            digitalWrite(lite_pin, HIGH); //off
        }
    }

    void blink_lt(unsigned int time)
    {
        digitalWrite(lite_pin, LOW); //on
        delta_time = time;
    }


    private:

    int lite_pin = 0;
    unsigned long last_time = 0;
    unsigned long delta_time = 0;

};


//for handling omniwheel base movement


/*

posititive direction, the wheels spin counterclockwise

Wheel base:

  front
3/     \2

   _
   1

//wheel 1's offset angle is 0
//2's offset angle is +240 deg (4pi/3 rad)
//3's offset angle is +120 deg (2pi/3 rad)

Unit circle

     PI/2
      |
      |
      |
PI------------0
      |
      |
      |
    3PI/2



Char Unit circle, radius +-512


      192
       |
       |
       |
0------------128
       |
       |
       |
       64


*/



class FastTrig
{
public:

	//don't ask where I got this class...
	//...CSE2. it was CSE2.

    //constructor and destructor
    FastTrig()
    {
        InitTriangleTable();
    }
    ~FastTrig() {}

    int GetSin(unsigned char deg)
    {
        return gSin[deg];
    }

    int GetCos(unsigned char deg)
    {
        deg += 0x40;
        return gSin[deg];
    }

	unsigned char GetArctan(int x, int y)
	{
		short k;
		unsigned char a;

		//use these to get coordiantes that work better with the screen coordiante system, where 0,0 is top left
		//x *= -1;
		//y *= -1;

		a = 0;

		//catch 0 values
		if (x == 0 && y == 0)
			return a;

		if (x > 0)
		{
			if (y > 0)
			{
				if (x > y)
				{
					k = (y * 0x2000) / x;
					while (k > gTan[a])
						++a;
				}
				else
				{
					k = (x * 0x2000) / y;
					while (k > gTan[a])
						++a;
					a = 0x40 - a;
				}
			}
			else
			{
				if (x > -y)
				{
					k = (-y * 0x2000) / x;
					while (k > gTan[a])
						++a;
					a = 0x100 - a;
				}
				else
				{
					k = (x * 0x2000) / -y;
					while (k > gTan[a])
						++a;
					a = 0x100 - 0x40 + a;
				}
			}
		}
		else
		{
			if (y > 0)
			{
				if (-x > y)
				{
					k = (y * 0x2000) / -x;
					while (k > gTan[a])
						++a;
					a = 0x80 - a;
				}
				else
				{
					k = (-x * 0x2000) / y;
					while (k > gTan[a])
						++a;
					a = 0x40 + a;
				}
			}
			else
			{
				if (-x > -y)
				{
					k = (-y * 0x2000) / -x;
					while (k > gTan[a])
						++a;
					a = 0x80 + a;
				}
				else
				{
					k = (-x * 0x2000) / -y;
					while (k > gTan[a])
						++a;
					a = 0x100 - 0x40 - a;
				}
			}
		}

		return a;
	}


	static float ConvertBackToRadians(unsigned char deg)
	{
		return (deg * 6.2831998 / 256.0);
	}

	static unsigned char ConvertToFastDegs(float radians)
	{
		return (unsigned char)floor(radians * 256.0 / (2.0 * PI));
	}


private:
    //SINE maps 0-2pi to 0-256, so 128 = pi and 64 = pi/2, etc.
    //COSINE simply offsets SINE by 0x40
    //the unit circle has a radius of 512

    int gSin[0x100];
    short gTan[0x21];


    void InitTriangleTable(void)
    {
        int i;


        //sin(2*pi/deg_count)*radius

        // Sine
        for (i = 0; i < 0x100; ++i)
            gSin[i] = (int)(sin(i * 6.2831998 / 256.0) * 512.0);

        float a, b;

        // Tangent
        for (i = 0; i < 0x21; ++i)
        {
            a = (float)(i * 6.2831855f / 256.0f);
            b = (float)sin(a) / (float)cos(a);
            gTan[i] = (short)(b * 8192.0f);
        }

    }







};


//does all the roboty things (like movement, weapon handling, etc.)
class RobotHandler
{

    public:

    RobotHandler();
    ~RobotHandler();

    void update();

    //disable everything (emergency kill mode when no transmitter packets are recieved)
    void pause();

    //re-enable everything
    void resume();

    //read voltage directly to internal variable (float form)
    void ReadVoltage();

    private:

    //we know this works, so use it for testing radio rx and tx
    void TestMain();

    void SendTelemetry();

    //for sending out telemetry packets
    unsigned long last_time = 0;

    //used to update the ESC output (only in dshot mode)
    unsigned long last_esc_time = 0;

    RadioNowHandler* radio;
    BlinkerHandler* blinker;
    FastTrig* mr_trig;



    DShotRMT* esc;
    Servo* servo_1;
    Servo* servo_2;


    //last packet read from the radio
    //concatated_channels_t gotten_data = {};
    
    //wheelbase vectors x, y, rotation
    int16_t xm, ym, rot_m= {};

    int16_t mot1, mot2, mot3 = {};
    float ramp_tune = {};

    //weapon location and speed
    int16_t servo_angle = 0;
    int16_t servo_angle_min = 0;
    //int16_t servo_angle_max = 0;

    int16_t temp_servo2_min = {};
    int16_t temp_servo2_max = {};

    uint16_t esc_get_speed = 0;
    uint16_t esc_set_speed = 0;
    uint8_t esc_reversed = 0; //CW vs CCW

    //remote disable
    bool remote_disable = false;
    //current enable state
    bool is_enabled = false;

    bool is_flipped_over = false;
    bool is_two_wheeled = false;
    uint8_t broken_wheel = 0;


    float_t battery_voltage = 0.0;

    //put rx data into each specific variable
    void MapControllerData();

    //write processed data to each output
    void SetWheelSpeedProportions();
    void WriteMotors();

    //test: dump channels to serial monitor
    void DumpChannelPacket();
    //dump processed cahnnels to serial moditor
    void DumpMappedPacket();

    //for non-proportional motor ramping
    float LogMap(float input, float i, float h, float strength);
    float TurnMap(float input, float t, float u, float degrees, float linear_slope);


};




#endif

