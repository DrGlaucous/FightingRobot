
#include <Arduino.h>
#include <PPMReader.h>


#include "configuration.h"
#include "TimerTick.h"


#ifdef IS_CONTROLLER
#include "Transmitter.h"
TransmitterHandler* transmitter;
#else
#include "Robot.h"
RobotHandler* robot;
#endif



void setup()
{
    Serial.begin(115200);
    Serial.printf("AABC\n");

    //needs to be initialized here because the radio depends on IRQs which cannot be declared out of the gate
#ifdef IS_CONTROLLER
    transmitter = new TransmitterHandler();
#else
    robot = new RobotHandler();
#endif


}


void loop()
{

    gTimer.update();

#ifdef IS_CONTROLLER
    transmitter->update();
#else
    robot->update();
#endif


}





