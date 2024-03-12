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
    if(TimerHandler::DeltaTimeMillis(&last_time, 5))
    {
        if(controller->update())
            return;

        remote_control_packet_t outbox = {};
        outbox.channels = controller->GetReadyPacket();
        radio->SendPacket(outbox);


        if(radio->CheckForPacket(NULL) == RX_SUCCESS)
        {
            led_state = !led_state;
            digitalWrite(LED_BUILTIN, led_state);
            Serial.println("Got Telemetry");
        }

    }
    
}


#endif

