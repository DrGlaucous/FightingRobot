

#include <Arduino.h>
#include <PPMReader.h>

#include "Controller.h"
#include "configuration.h"



ControllerHandler::ControllerHandler()
{

}

ControllerHandler::~ControllerHandler()
{
    
}


void update()
{

}


void NormalizePPM(uint16_t* channels, uint16_t len)
{
    //overflow protection
    if(len > CHANNEL_COUNT)
        len = CHANNEL_COUNT - 1;





}























