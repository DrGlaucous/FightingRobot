
#include <Arduino.h>

#include <esp_now.h>
#include <esp_wifi.h>
#include <Wifi.h>

#include "configuration.h"
#include "TimerTick.h"
#include "Controller.h"
#include "ESPRadio.h"


//custom MAC addresses
uint8_t sender_addr[] = {0x30, 0xAE, 0xA4, 0x07, 0x0D, 0x64};
uint8_t rec_addr[] = {0xA0, 0x0E, 0x04, 0x0F, 0xFD, 0x64};


TimerHandler gTimer; 
RadioNowHandler* gRadio;


#ifdef IS_CONTROLLER
ControllerHandler* gController;
#endif



void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

#ifdef IS_CONTROLLER
    gController = new ControllerHandler();
#endif


    gRadio = new RadioNowHandler();

}


void loop()
{

    gTimer.update();


    static unsigned long delta_t = 0;
#ifdef IS_CONTROLLER
    gController->update();

    static unsigned int l_state = 0;
    if(gTimer.DeltaTimeMillis(&delta_t, 5))
    {
        digitalWrite(LED_BUILTIN, l_state % 2);
        ++l_state;
        remote_control_packet_t outbox = {};
        outbox.channels = gController->GetReadyPacket();
        gRadio->SendPacket(outbox);


        if(gRadio->CheckForPacket(NULL) == RX_SUCCESS)
        {
            Serial.println("Got Telemetry");
        }

    }
#else

    //send telemetry back on a regular interval
    if(gTimer.DeltaTimeMillis(&delta_t, 100))
    {
        remote_ack_packet_t outbox = {};
        outbox.battery_voltage = 11.7;
        outbox.motor_rpm = 12;
        gRadio->SendPacket(outbox);

    }

    if(gRadio->CheckForPacket(NULL) == RX_SUCCESS)
    {
        auto commands = gRadio->GetLastControlPacket();
        Serial.printf("%d || %d ||-|| %d || %d\n", commands.channels.analog_channels[2], commands.channels.analog_channels[3], commands.channels.digital_channels[0], commands.channels.digital_channels[1]);
    }


#endif



}





