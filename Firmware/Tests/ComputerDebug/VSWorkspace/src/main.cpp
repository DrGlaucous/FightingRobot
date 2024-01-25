#include <stdio.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>


#define PI 3.1415926535


/*

posititive direction, the wheels spin counterclockwise

Wheel base:

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



Char Unit circle, radius +-256


       64
       |
       |
       |
128------------0
       |
       |
       |
      192


*/


class FastTrig
{
public:

	//don't ask where I got this class...


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

	unsigned char GetArktan(int x, int y)
	{
		short k;
		unsigned char a;

		x *= -1;
		y *= -1;

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


	float ConvertBackToRadians(unsigned char deg)
	{
		return (deg * 6.2831998 / 256.0);
	}

	unsigned char ConvertToFastDegs(float radians)
	{
		return (unsigned char)floor(radians * 256 / (2 * PI));
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




void setWheelSpeedProportions(float* wheel_1, float* wheel_2, float* wheel_3, float angle)
{
    //float x_componet = cos(angle);
    //float y_componet = sin(angle);

	FastTrig mrQuick;



    *wheel_1 = cos(angle);
    *wheel_2 = cos(angle + 4 * PI / 3.0);
    *wheel_3 = cos(angle + 2 * PI / 3.0);

}



int main(void)
{
    float angle = PI/2.0;// PI / 3;
    float wheel_1;
    float wheel_2;
    float wheel_3;
    setWheelSpeedProportions(&wheel_1, &wheel_2, &wheel_3, angle);



    float stopper = angle + 1.0;

    return 1;



}





















