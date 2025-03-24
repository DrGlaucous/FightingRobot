
#include <Arduino.h>

#include <esp_now.h>
#include <esp_wifi.h>
#include <Wifi.h>
#include <DShotRMT.h>

#include "configuration.h"
#include "TimerTick.h"

#include "Robot.h"


RobotHandler* robot = nullptr;


void setup()
{
    Serial.begin(115200);

    robot = new RobotHandler();

}


void loop()
{
    robot->update();


}





