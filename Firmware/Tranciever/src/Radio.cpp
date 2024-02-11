

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


//assemble radio response from buffer
int RadioHandler::AssembleResponse(packet_type_t* packet_type)
{
    //figure out what packet we got
    packet_type_t rx_type;
    memcpy(&rx_type, radio.DATA, sizeof(packet_type_t));

    //copy gotten packet to the cache
    switch(rx_type)
    {
        default: //do nothing if packet is not a predefined type
            return -1; //improper packet
            break;
        case PACKET_CONTROL_VALUES:
        {
            memcpy(&last_gotten_control, radio.DATA, radio.DATALEN);
            last_rssi = radio.RSSI;
        }
        break;
        case PACKET_RESPONSE_VALUES:
        {
            Serial.printf("Got ACK\n");
            memcpy(&last_gotten_response, radio.DATA, radio.DATALEN);
            last_rssi = radio.RSSI;
        }
        break;
    }

    //pass type back to caller
    if(packet_type)
        *packet_type = rx_type;

    //send data back if ACK was requested
    if (radio.ACKRequested())
        SendResponsePacket();

    return 0; //success
}

//get and assemble a response from the radio
int RadioHandler::CheckForResponse(packet_type_t* packet_type)
{
    //packet waiting in queue
    if(radio.receiveDone())
    {
        return AssembleResponse(packet_type);
    }

    return 1; //no packet to get
}


remote_response_packet_t RadioHandler::GetLastResponsePacket()
{ return last_gotten_response;}
remote_control_packet_t RadioHandler::GetLastControlPacket()
{ return last_gotten_control;}


//custom ACK handler
void RadioHandler::SendResponsePacket()
{
    //filler data for now
    remote_response_packet_t back_pack = {};
    back_pack.hi_there = 16;

    Serial.printf("Sent ACK\n");

    radio.sendACK(&back_pack, sizeof(remote_response_packet_t));

}



//int RadioHandler::SendPacket(void* packet, size_t size, uint8_t destination, bool ack)
int RadioHandler::SendPacket(remote_control_packet_t packet, uint8_t destination, bool ack)
{
    //enforce packettype
    packet.packettype = PACKET_CONTROL_VALUES;
    uint8_t size = sizeof(packet);

    if(ack)
    {
        if(radio.sendWithRetry(destination, &packet, size))
        {
            //got ack
            
            //parse ACK response
            AssembleResponse(NULL);

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








