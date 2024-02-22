#include <Arduino.h>
#include <PPMReader.h>


#include "configuration.h"
#include "Radio.h"
#include "Controller.h"
#include "TimerTick.h"
#include "Transmitter.h"

#ifdef IS_CONTROLLER



TransmitterHandler::TransmitterHandler()
{
    auto startup_settings = radio_handler_config_datapack_t
    {
        SLAVE_PIN,
        IRQ_PIN,
        FREQUENCY,
        TRANSMITTERNODEID,
        NETWORKID,
        IS_HIGH_POWER,
        ENCRYPT,
        ENCRYPTKEY,
    };
    radio = new RadioHandler(startup_settings);

    controller = new ControllerHandler();

}

TransmitterHandler::~TransmitterHandler()
{
    delete(radio);
    delete(controller);
}


void TransmitterHandler::update()
{
    //5 ms have elapsed
    if(gTimer.DeltaTimeMillis(&last_time, 5))
    {
        controller->update();

        //transmit packet

        //ready packet
        remote_control_packet_t packet_out = {};
        packet_out.channels = controller->GetReadyPacket();
        //packet_out.channels.analog_channels[0] = 303;


        //if transmit success print the bounceback time
        if(!radio->SendPacket(packet_out, RECEIVERNODEID, USEACK))
        {
            Serial.printf("%f\n", radio->GetLastResponsePacket().battery_voltage);
        }
    }
}


#endif

