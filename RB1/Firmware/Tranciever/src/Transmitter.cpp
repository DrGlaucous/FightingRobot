#include <Arduino.h>
#include <PPMReader.h>


#include "configuration.h"
#include "Controller.h"
#include "TimerTick.h"
#include "Transmitter.h"
#include "ESPRadio.h"

#ifdef IS_CONTROLLER


TransmitterHandler::TransmitterHandler()
{
    radio = new RadioNowHandler();
    controller = new ControllerHandler();

    pinMode(LED_BUILTIN, OUTPUT);
}

TransmitterHandler::~TransmitterHandler()
{
    delete(radio);
    delete(controller);
}


void TransmitterHandler::update()
{

    //controller->update();



    //5 ms have elapsed
    //if(TimerHandler::DeltaTimeMillis(&last_time, 5))

    //instead, send at the soonest update opportunity (first stable packet collected)
    if(controller->update() == UPDATE_GOOD)
    {

        remote_control_packet_t outbox = {};
        outbox.channels = controller->GetReadyPacket();

        radio->SendPacket(outbox);


        if(radio->CheckForPacket(NULL) == RX_SUCCESS)
        {
            led_state = !led_state;
            digitalWrite(LED_BUILTIN, led_state);
            //Serial.println("Got Telemetry");
            remote_ack_packet_t ackpack = radio->GetLastAckPacket();
            Serial.printf("Volt: %5.3f, RPM: %d\n", ackpack.battery_voltage, ackpack.motor_rpm);
        }

    }
    
}


#endif

