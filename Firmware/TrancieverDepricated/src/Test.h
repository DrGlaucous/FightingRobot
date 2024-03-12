// #pragma once

// #include <Arduino.h>
// #include <PPMReader.h>

// #include "TimerTick.h"
// #include "configuration.h"
// #include "Radio.h"



// class Tester
// {

// public:
//     Tester()
//     {
//         auto startup_settings = radio_handler_config_datapack_t
//         {
//             SLAVE_PIN,
//             IRQ_PIN,
//             FREQUENCY,
//             2, //my node
//             NETWORKID, //network
//             IS_HIGH_POWER,
//             false,
//             ENCRYPTKEY,
//         };

//         radio = new RadioHandler(startup_settings);
//     }

//     ~Tester()
//     {
//         delete(radio);
//     }

//     void update()
//     {
//         if(gTimer.MillisChanged() && gTimer.DeltaTimeMillis(&last_sent, millis_time))
//         {
//             packet.channels[0] = 1280;
//             radio->SendRCPacket(packet, 1, false);
//             Serial.println("Sent packet.");
//         }
        
//         packet_type_t packk_backk = {};
//         radio->CheckForResponse(&packk_backk);
//         if(packk_backk != 0)
//         {
//             Serial.println("Got packet");
//         }

//     }

// private:

//     RadioHandler* radio;

//     //holds the most recent packet gotten from the remote
//     remote_control_packet_t packet = {};
    
//     //packet to send off to the remote
//     remote_response_packet_t packet_out = {};

//     //how long it's been since we got a packet from the remote (good for use as a keepalive signal)
//     unsigned long last_rx_time{};

//     unsigned long last_sent{};
//     unsigned long millis_time = 1000;

// };






















