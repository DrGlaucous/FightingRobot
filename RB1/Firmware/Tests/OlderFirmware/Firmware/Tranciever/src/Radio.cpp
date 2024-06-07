

#include <Arduino.h>
#include <RFM69.h>

#include "Radio.h"
#include "configuration.h"



RadioHandler::RadioHandler(radio_handler_config_datapack_t settings)
{

    radio.setCS(settings.slave_sel_pin);
    radio.setIrq(settings.irq_pin);
    radio.initialize(settings.frequency, settings.this_id, settings.network_id);

    if(settings.should_encrypt)
        radio.encrypt(settings.encrypt_key);
}

RadioHandler::~RadioHandler()
{
    //nothing to do for now
}

//get and assemble a response from the radio
int RadioHandler::CheckForResponse(packet_type_t* gotten_packet)
{
    //packet waiting in queue
    if(radio.receiveDone())
    {
        //figure out what packet we got
        switch(radio.DATALEN)
        {
            default: //do nothing if packet is not a predefined type
                return -1; //improper packet
                break;
            case sizeof(remote_control_packet_t):
            {
                memcpy(&last_gotten_control, radio.DATA, radio.DATALEN);
                last_rssi = radio.RSSI;
                if(gotten_packet)
                    *gotten_packet = PACKET_CONTROL_VALUES;
            }
            break;
            case sizeof(remote_response_packet_t):
            {
                memcpy(&last_gotten_response, radio.DATA, radio.DATALEN);
                last_rssi = radio.RSSI;
                if(gotten_packet)
                    *gotten_packet = PACKET_RESPONSE_VALUES;
            }
            break;
        }

        if (radio.ACKRequested())
            radio.sendACK();

        return 0; //success
    }

    return 1; //no packet to get
}


remote_response_packet_t RadioHandler::GetLastResponsePacket()
{ return last_gotten_response;}
remote_control_packet_t RadioHandler::GetLastControlPacket()
{ return last_gotten_control;}


//basic packet sending functions
int RadioHandler::SendRCPacket(remote_control_packet_t packet, uint8_t destination, bool ack)
{
    //enforce packettype
    packet.packettype = PACKET_CONTROL_VALUES;
    int size = sizeof(packet);
    return SendPacket(&packet, size, destination, ack);
}

int RadioHandler::SendResponsePacket(remote_response_packet_t packet, uint8_t destination, bool ack)
{
    //enforce packettype
    packet.packettype = PACKET_RESPONSE_VALUES;
    int size = sizeof(packet);
    return SendPacket(&packet, size, destination, ack);
}

int RadioHandler::SendPacket(void* packet, size_t size, uint8_t destination, bool ack)
{
    if(ack)
    {
        if(radio.sendWithRetry(destination, packet, size))
        {
            //got ack
            return 0; //success
        }
        else
        {
            //no ack
            return -1; //fail
        }
    }
    else
    {
        radio.send(destination, &packet, size);
        return 0; //success
    }

}








