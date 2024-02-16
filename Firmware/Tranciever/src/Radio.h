#pragma once

#include <Arduino.h>
#include <RFM69.h>

#include "configuration.h"

typedef enum packet_type_e
{
    PACKET_NULL = 0,
    PACKET_CONTROL_VALUES,
    PACKET_RESPONSE_VALUES,
}packet_type_t;


typedef enum response_status_e
{
    RX_SUCCESS = 0,
    RX_SENT_ACK,
    RX_BAD_FORMAT,
    RX_NOTHING,
}response_status_t;

//packet sent from the controller to the machine

// To force compiler to use 1 byte packaging 
#pragma pack(1) 
typedef struct remote_control_packet_s
{
    packet_type_t packettype = PACKET_CONTROL_VALUES;
    uint16_t channels[CHANNEL_COUNT] = {};

    uint16_t analog_channels[ANALOG_CHANNEL_CNT] = {};
    //2 switches per each digital channel
    uint8_t digital_channels[DIGITAL_CHANNEL_CNT * 2] = {};

}remote_control_packet_t;

//packet sent from the machine to the controller
#pragma pack(1) 
typedef struct remote_ack_packet_s
{
    packet_type_t packettype = PACKET_RESPONSE_VALUES;
    uint64_t hi_there = 0;
}remote_ack_packet_t;

//all the settings for configuring the radio
typedef struct radio_handler_config_datapack_s
{
    uint8_t slave_sel_pin; //pin attacked to the slave select
    uint8_t irq_pin; //pin attached to IRQ
    uint8_t frequency; //pre-defined frequencies only, please
    uint8_t this_id; //id of this device on the network
    uint8_t network_id; //same for all devices on the network
    bool is_hw; //is the high-power variant
    bool should_encrypt; //use the encryption key
    char encrypt_key[17]; //key must be 16 bytes long (+1 for null terminator)

}radio_handler_config_datapack_t;

class RadioHandler
{
    public:
    RadioHandler(radio_handler_config_datapack_t settings);
    ~RadioHandler();

    int SendPacket(remote_control_packet_t packet, uint8_t destination, bool ack);

    response_status_t CheckForResponse(packet_type_t* rx_packet_type = NULL, remote_ack_packet_t ack_packet = {});

    remote_ack_packet_t GetLastResponsePacket();
    remote_control_packet_t GetLastControlPacket();

    private:

    //int SendPacket(void* packet, size_t size, uint8_t destination, bool ack);

    //only used to send ACK packets
    void SendResponsePacket(remote_ack_packet_t ack_packet);

    //parse response type
    response_status_t AssembleRX(packet_type_t* packet_type, remote_ack_packet_t ack_packet = {});

    //radio module
    RFM69 radio;

    //these are where the fresh values are placed when a response is recieved
    remote_ack_packet_t last_gotten_response = {};
    remote_control_packet_t last_gotten_control = {};
    int16_t last_rssi = 0; //RSSI rating of the last packet

};

















