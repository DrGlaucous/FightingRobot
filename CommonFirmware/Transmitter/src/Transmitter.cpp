#include <Arduino.h>


#include "configuration.h"
#include "Controller.h"
#include "TimerTick.h"
#include "Transmitter.h"
#include "ESPRadio.h"

TransmitterHandler::TransmitterHandler()
{
    radio = new RadioNowHandler();
    controller = new ControllerHandler();

    //LED_BUILTIN
    pinMode(0, OUTPUT);
}

TransmitterHandler::~TransmitterHandler()
{
    delete(radio);
    delete(controller);
}


void TransmitterHandler::update()
{

    //controller->update();


    // //TEST: send a spoof packet
    // if(TimerHandler::DeltaTimeMillis(&last_time, 1000)) {
    //     remote_control_packet_t outbox = remote_control_packet_t{};
    //     outbox.channels.analog_channels[0] = 64;
    //     radio->SendPacket(outbox);
    //     Serial.printf("sent\n");
    // }
    // return;



    //instead, send at the soonest update opportunity (first stable packet collected)
    if(controller->update() == UPDATE_GOOD)
    {

        Serial.printf("Update Good\n");

        remote_control_packet_t outbox = {};
        outbox.channels = controller->GetReadyPacket();

        radio->SendPacket(outbox);


        if(radio->CheckForPacket(NULL) == RX_SUCCESS)
        {
            led_state = !led_state;
            digitalWrite(0, led_state);
            //Serial.println("Got Telemetry");
            remote_ack_packet_t ackpack = radio->GetLastAckPacket();
            Serial.printf("Volt: %5.3f, RPM: %d\n", ackpack.battery_voltage, ackpack.motor_rpm);
        }

    }
    
}

