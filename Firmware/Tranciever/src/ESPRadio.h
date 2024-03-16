#pragma once
#include <Arduino.h>

#include <esp_now.h>
#include <esp_wifi.h>
#include <Wifi.h>

#include "Controller.h"
#include "configuration.h"



//note: this is for readability, the packet must be cast to a char for transmission
//less of a problem now that both devices are using the same compiler
typedef enum packet_type_e
{
    PACKET_NULL = 0x00,
    PACKET_CONTROL_VALUES = 0x01,
    PACKET_RESPONSE_VALUES = 0x02,
    PACKET_INTEGER = 0x03,
    PACKET_ARRAY = 0x04,
}packet_type_t;


typedef enum rx_status_e
{
    RX_SUCCESS = 0,
    RX_QUEUE_EMPTY,

}rx_status_t;
typedef enum tx_status_e
{
    TX_SUCCESS = 0,
    TX_FAIL = 0,
}tx_status_t;


//packet sent from the controller to the machine
#pragma pack(1) //force compiler to compact the struct
typedef struct remote_control_packet_s
{
    concatated_channels_t channels = {};
}remote_control_packet_t;



//packet sent from the machine to the controller
#pragma pack(1) 
typedef struct remote_ack_packet_s
{
    uint32_t motor_rpm = 0;
    float_t battery_voltage = 0.0;

}remote_ack_packet_t;

class RadioNowHandler
{
    public:
    RadioNowHandler();
    ~RadioNowHandler();

    //todo: inline these
    tx_status_t SendPacket(unsigned int data);
    tx_status_t SendPacket(remote_control_packet_t data);
    tx_status_t SendPacket(remote_ack_packet_t data);
    tx_status_t SendPacket(void* data, uint32_t size);

    remote_control_packet_t GetLastControlPacket();
    remote_ack_packet_t GetLastAckPacket();
    uint64_t GetDeltaTime();

    rx_status_t CheckForPacket(packet_type_t* last_got);

    private:

    tx_status_t SendPacket(void* data, uint32_t size, packet_type_t type);
    
    //uint64_t delta_time = {};

    uint8_t raw_queue_dump[ESP_NOW_MAX_DATA_LEN] = {};
    remote_control_packet_t last_control_packet = {};
    remote_ack_packet_t lask_ack_packet = {};



};




























