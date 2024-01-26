#pragma once

#include <Arduino.h>
#include <PPMReader.h>

#include "configuration.h"
#include "Radio.h"

#ifndef IS_CONTROLLER

class ReceiverHandler
{

public:
    ReceiverHandler();
    ~ReceiverHandler();

    void update();

private:

    RadioHandler* radio;

    //holds the most recent packet gotten from the remote
    remote_control_packet_t packet_in = {};
    
    //packet to send off to the remote
    remote_response_packet_t packet_out = {};

    //how long it's been since we got a packet from the remote (good for use as a keepalive signal)
    unsigned long last_rx_time{};

    unsigned long last_sent{};
    unsigned long millis_time = 1000;

};


#endif

