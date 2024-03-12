//class that handles all the radio and PPM operations for the transmitter module

#pragma once

#include <Arduino.h>
#include <PPMReader.h>


#include "configuration.h"
#include "ESPRadio.h"
#include "Controller.h"


#ifdef IS_CONTROLLER

class TransmitterHandler
{

    public:
    TransmitterHandler();
    ~TransmitterHandler();

    void update();



    private:

    //for timing transmissions
    unsigned long last_time = 0;

    RadioNowHandler* radio;
    ControllerHandler* controller;


};


#endif



