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


*/


void setWheelSpeedProportions(float* wheel_1, float* wheel_2, float* wheel_3, float angle)
{
    //float x_componet = cos(angle);
    //float y_componet = sin(angle);

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





















