#pragma once

#include <Arduino.h>
#include <PPMReader.h>

#include "configuration.h"
#include "Radio.h"


#ifdef IS_CONTROLLER

class TransmitterHandler
{

public:
    TransmitterHandler();
    ~TransmitterHandler();

    void update();

private:

    RadioHandler* radio;
    PPMReader* ppm;

    //sendoff (we put this here so we can gauard against bad packets)
    remote_control_packet_t packet_out = {};

    //holds the most recent packet gotten from the robot (unused for now)
    remote_response_packet_t packet_in = {};

    unsigned long last_sent{};
    unsigned long millis_time = 1000;

};




#endif




