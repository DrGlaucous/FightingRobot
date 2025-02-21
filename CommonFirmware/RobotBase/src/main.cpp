
#include <Arduino.h>

#include <esp_now.h>
#include <esp_wifi.h>
#include <Wifi.h>

#include "configuration.h"
#include "TimerTick.h"
#include "Transmitter.h"
#include "Robot.h"
#include "PPMRMT.h"

//custom MAC addresses
uint8_t sender_addr[] = {0x30, 0xAE, 0xA4, 0x07, 0x0D, 0x64};
uint8_t rec_addr[] = {0xA0, 0x0E, 0x04, 0x0F, 0xFD, 0x64};





RobotHandler* robot;
//PPMRMT* ppm_get;

void setup()
{
    Serial.begin(115200);


    robot = new RobotHandler();


    //ppm_get = new PPMRMT(PPM_INTURRUPT_PIN, PPM_CHANNEL_COUNT);

}


void loop()
{


    robot->update();


    // static unsigned long delta_t = 0;
    // //send telemetry back on a regular interval
    // if(TimerHandler::DeltaTimeMillis(&delta_t, 100))
    // {
    //     remote_ack_packet_t outbox = {};
    //     outbox.battery_voltage = 11.7;
    //     outbox.motor_rpm = 12;
    //     gRadio->SendPacket(outbox);
    // }
    // if(gRadio->CheckForPacket(NULL) == RX_SUCCESS)
    // {
    //     auto commands = gRadio->GetLastControlPacket();
    //     Serial.printf("%d || %d ||-|| %d || %d\n", commands.channels.analog_channels[2], commands.channels.analog_channels[3], commands.channels.digital_channels[0], commands.channels.digital_channels[1]);
    // }


    //ppm_get->get_latest_ppm(0);

}





