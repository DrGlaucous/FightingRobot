#include <Arduino.h>

#include "RFM69HW.h"
#include "configuration.h"
#include "Controller.h"
#include "TimerTick.h"
#include "Transmitter.h"
#include "ESPRadio.h"

TransmitterHandler::TransmitterHandler()
{
    radio = new RadioNowHandler();
    //controller = new ControllerHandler();
    rfm = new RFM_RadioHandler();

    SPI.begin(RFM_SCK, RFM_MISO, RFM_MOSI);
    rfm->begin(&SPI, RFM_NSS, RFM_RESET, RFM_ISR, RFM_MYNODEID, RFM_NETWORKID, ENCRYPTKEY_P);


    //LED_BUILTIN
    //pinMode(0, OUTPUT);


}

TransmitterHandler::~TransmitterHandler()
{
    if(radio) {
        delete(radio);
        radio = nullptr;
    }
    if(controller) {
        delete(controller);
        controller = nullptr;
    }
    if(rfm) {
        delete(rfm);
        rfm = nullptr;
    }
}


void TransmitterHandler::update()
{



    //instead, send at the soonest update opportunity (first stable packet collected)
    //if(controller->update() == UPDATE_GOOD)
    if(false)
    {

        //Serial.printf("Update Good\n");

        remote_control_packet_t outbox = {};
        outbox.channels = controller->GetReadyPacket();


        //rfm mode
        // {
        //     ptype out_type = RFM_CONTROL_TYPE;
        //     RFM_Packet outgoing_packet = {};
        //     for(int i = 0; i < DIGITAL_CHANNELS_CT; ++i) {
        //         outgoing_packet.control.digital_switches[i] = outbox.channels.digital_channels[i];
        //     }
        //     for(int i = 0; i < ANALOG_CHANNELS_CT; ++i) {
        //         outgoing_packet.control.analog_channels[i] = outbox.channels.analog_channels[i];
        //     }
        //     rfm->sendPacket(&outgoing_packet, out_type, RFM_TONODEID, false);
        // }



        //Wifi mode
        radio->SendPacket(outbox);
        if(radio->CheckForPacket(NULL) == RX_SUCCESS)
        {
            //led_state = !led_state;
            //digitalWrite(0, led_state);
            
            //Serial.println("Got Telemetry");
            remote_ack_packet_t ackpack = radio->GetLastAckPacket();
            Serial.printf("Volt: %5.3f, RPM: %d\n", ackpack.battery_voltage, ackpack.motor_rpm);
        }

    }
    

    else {
        ptype out_type = RFM_CONTROL_TYPE;
        RFM_Packet outgoing_packet = {};

        rfm->sendPacket(&outgoing_packet, out_type, RFM_TONODEID, false);

    }
}

