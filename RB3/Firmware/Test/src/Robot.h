#pragma once
#include <Arduino.h>

#include "Controller.h"
#include "ESPRadio.h"
#include "configuration.h"
#include "TimerTick.h"

#include <ESP32Servo.h>
#include <DShotRMT.h>




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

class PIDController {

    public:

    PIDController(
        int32_t start_output_offset = DSHOT_THROTTLE_MIN,
        int32_t max_lim = DSHOT_THROTTLE_MAX,
        int32_t min_lim = DSHOT_THROTTLE_MIN,
        float p_const = 0.2,
        float i_const = 0.2,
        float d_const = 0.2
    ) {
        if(start_output_offset > max_lim)
            start_output_offset = max_lim;

        this->start_output_offset = start_output_offset;
        this->max_lim = max_lim;
        this->min_lim = min_lim;

        this->p_const = p_const;
        this->i_const = i_const;
        this->d_const = d_const;

    };

    ~PIDController() {
    };

    //runs the pid loop for one cycle (uses floats, non-ISR-safe)
    int32_t tick(uint32_t target_rpm, uint32_t curr_rpm, uint32_t delta_micros) {

        //small RPM = big throttle value, direct acting

        //delta_micros = 200; //for testing

        //determine PID values

        float pval = (float)target_rpm - (float)curr_rpm;

        //if our value is increasing/decreasing and we limited the output, don't continue integration
        //or if our current value is 0 (rely only on P control for startup)
        if(
            !(
            (pval > 0.0 && has_limited_high) ||
            (pval < 0.0 && has_limited_low)
            )
            && curr_rpm != 0
        ) {
            ival += pval * i_const * (float)delta_micros;
        }

        float dval = pval - last_pval;
        last_pval = pval;

        //sum and multiply by constants
        int32_t pid_out = (int32_t)(
            pval * p_const * (float)delta_micros
            + ival //integral constant is applied above
            + dval * d_const * (float)delta_micros
        ) + start_output_offset;

        //limit output
        if(pid_out > max_lim) {
            pid_out = max_lim;
            has_limited_high = true;
            has_limited_low = false;
        } else if (pid_out < min_lim) {
            pid_out = min_lim;
            has_limited_high = false;
            has_limited_low = true;
        } else {
            has_limited_high = false;
            has_limited_low = false;
        }

        return(pid_out);
    
    }

    //called whenever we take manual control of the loop, resets persistent I and D values
    void reset() {
        last_pval = 0;
        ival = 0;
        has_limited_high = false;
        has_limited_low = false;
    }

    //controls what extra value will be added to the loop, typically for starting at "full throttle"
    void set_output_offset(int32_t offset) {
        if(offset < min_lim) {
            start_output_offset = min_lim;
        } else if(offset > max_lim) {
            start_output_offset = max_lim;
        } else {
            start_output_offset = offset;
        }
    }


    int32_t test(uint32_t target_rpm, uint32_t curr_rpm, uint32_t delta_micros) {


        Serial.printf("A TEST %d\n", target_rpm);

        float pval = (float)target_rpm;

        //test
        Serial.printf("==%f==1\n", pval);
        pval = 2.0;
        
        Serial.printf("==%f==2\n", pval);
        return 0;
    }


    private:

    //vars
    float p_const = 0.0;
    float i_const = 0.0;
    float d_const = 0.0;

    //data retainers
    float last_pval = 0;
    float ival = 0;

    int32_t max_lim = DSHOT_THROTTLE_MAX;
    int32_t min_lim = DSHOT_THROTTLE_MIN;

    int32_t start_output_offset = 0;


    bool has_limited_high = false;
    bool has_limited_low = false;

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


    DShotRMT* esc;


    //last packet read from the radio
    //concatated_channels_t gotten_data = {};
    
    //wheelbase vectors left side and right side
    int16_t left, right {};

    //actual motor output (from PID)
    int16_t mot1, mot2 = {};


    //weapon location and speed

    uint16_t esc_get_speed = 0;
    uint16_t esc_set_speed = 0;

    //remote disable
    bool remote_disable = false;
    //current enable state
    bool is_enabled = false;


    //put rx data into each specific variable
    void MapControllerData();

    //write processed data to each output
    void SetWheelSpeedProportions();
    void WriteMotors();

    //test: dump channels to serial monitor
    void DumpChannelPacket();
    //dump processed cahnnels to serial moditor
    void DumpMappedPacket();



};



