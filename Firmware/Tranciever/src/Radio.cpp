

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
response_status_t RadioHandler::AssembleRX(packet_type_t* packet_type,  remote_ack_packet_t ack_packet)
{
    //figure out what packet we got
    packet_type_t rx_type;
    memcpy(&rx_type, radio.DATA, sizeof(packet_type_t));

    //copy gotten packet to the cache
    switch(rx_type)
    {
        default: //do nothing if packet is not a predefined type
            return RX_BAD_FORMAT; //improper packet
            break;
        case PACKET_CONTROL_VALUES:
        {
            memcpy(&last_gotten_control, radio.DATA, radio.DATALEN);
            last_rssi = radio.RSSI;
        }
        break;
        case PACKET_RESPONSE_VALUES:
        {
            //Serial.printf("Got ACK\n");
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
    {
        SendResponsePacket(ack_packet);
        return RX_SENT_ACK;
    }

    return RX_SUCCESS; //success
}

//get and assemble a response from the radio
response_status_t RadioHandler::CheckForResponse(packet_type_t* rx_packet_type, remote_ack_packet_t ack_packet)
{
    //packet waiting in queue
    if(radio.receiveDone())
    {
        //update keepalive time
        last_rx_millis = millis();

        return AssembleRX(rx_packet_type, ack_packet);
    }

    return RX_NOTHING; //no packet to get
}


remote_ack_packet_t RadioHandler::GetLastResponsePacket()
{ return last_gotten_response;}
remote_control_packet_t RadioHandler::GetLastControlPacket()
{ return last_gotten_control;}

//delta time
uint64_t RadioHandler::GetTimeSinceLastPacket()
{
    uint64_t time_now = millis();
    //detect overflows
    if(time_now < last_rx_millis)
        return time_now + (UINT64_MAX - last_rx_millis);
    else
        return time_now - last_rx_millis;


}


//custom ACK handler
void RadioHandler::SendResponsePacket(remote_ack_packet_t ack_packet)
{
    //Serial.printf("Sent ACK\n");

    radio.sendACK(&ack_packet, sizeof(remote_ack_packet_t));

}


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
            
            //parse ACK response, values are automatically stowed, so we don't need to give it anything
            AssembleRX(NULL);

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








