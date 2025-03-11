#include <Arduino.h>

#include <esp_now.h>
#include <esp_wifi.h>
#include <Wifi.h>

#include "ESPRadio.h"
#include "configuration.h"

bool RadioHandlerExsists = false;
QueueHandle_t gotten_data_holder;
//do not write to this variable
unsigned long rec_time = {};
//unsigned long last_rec_time = {};

void  PacketSentCallback(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    //Serial.print("\r\nLast Packet Send Status:\t");
    //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
void  PacketGotCallback(const uint8_t * mac, const uint8_t *incomingData, int len)
{
    //Serial.println("Got");
    //Serial.printf("Bytes received: %d, Num: %d\n", len, (int)*incomingData);
    //last_rec_time = rec_time;
    rec_time = millis();
    BaseType_t high_task_wakeup = pdFALSE;
    xQueueOverwriteFromISR(gotten_data_holder, incomingData, &high_task_wakeup);

}

//delta time
unsigned long deltaTime(unsigned long now, unsigned long last)
{
    //detect overflows
    if(now < last)
        return now + (UINT64_MAX - last);
    else
        return now - last;

}


RadioNowHandler::RadioNowHandler()
{
    if(RadioHandlerExsists)
    {
        //err
        return;
    }

    RadioHandlerExsists = true;

    //initialize the queue if it is not already active
    if(gotten_data_holder == NULL)
    {
        //create the callback data queue if it's not already present
        gotten_data_holder = xQueueCreate(1, ESP_NOW_MAX_DATA_LEN);
    }

    //check again for successful creation
    // if(gotten_data_holder == NULL)
    // {
    //     //err
    // }


    //setup the radios
    WiFi.mode(WIFI_STA);

    //Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    //esp_now_register_send_cb(PacketSentCallback);

    const uint8_t new_mac[] = MY_ADDRESS;
    esp_now_peer_info_t peer = {PEER_ADDRESS, ENCRYPTKEY_S};

    peer.channel = NETWORKID;
    peer.encrypt = ENCRYPT;

    if(ENCRYPT)
        esp_now_set_pmk((uint8_t*)ENCRYPTKEY_P);


    esp_wifi_set_mac(WIFI_IF_STA, new_mac);


    switch(esp_wifi_set_channel(NETWORKID, WIFI_SECOND_CHAN_ABOVE))
    {
        default:
        case 0:
            Serial.println("Success!");
            break;
        case ESP_ERR_WIFI_NOT_INIT:
            Serial.println("ESP_ERR_WIFI_NOT_INIT!");
            break;

        case ESP_ERR_WIFI_IF:
            Serial.println("ESP_ERR_WIFI_IF!");
            break;
        case ESP_ERR_INVALID_ARG:
            Serial.println("ESP_ERR_INVALID_ARG!");
            break;

    }

    if (esp_now_add_peer(&peer) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }
    
    //esp_now_register_recv_cb(PacketGotCallback);
    
    Serial.print("[NEW] ESP32 Board MAC Address:  ");
    Serial.println(WiFi.macAddress());

}


RadioNowHandler::~RadioNowHandler()
{
    //delete and derefrence queue
    vQueueDelete(gotten_data_holder);
    gotten_data_holder = NULL;

    RadioHandlerExsists = false;

    esp_now_unregister_send_cb();
    esp_now_unregister_recv_cb();
    esp_now_deinit();
}


tx_status_t RadioNowHandler::SendPacket(remote_ack_packet_t data)
{
    return SendPacket(&data, sizeof(data), PACKET_CONTROL_VALUES);
}
tx_status_t RadioNowHandler::SendPacket(remote_control_packet_t data)
{
    return SendPacket(&data, sizeof(data), PACKET_CONTROL_VALUES);
}
tx_status_t RadioNowHandler::SendPacket(void* data, uint32_t size)
{
    return SendPacket(data, size, PACKET_ARRAY);
}
tx_status_t RadioNowHandler::SendPacket(unsigned int data)
{
    return SendPacket(&data, sizeof(data), PACKET_INTEGER);
}
tx_status_t RadioNowHandler::SendPacket(void* data, uint32_t size, packet_type_t type)
{
    if(sizeof(type) + size > ESP_NOW_MAX_DATA_LEN)
        return TX_FAIL;
    
    uint8_t holder[ESP_NOW_MAX_DATA_LEN] = {};
    
    memcpy(holder, &type, sizeof(type));
    memcpy(holder + sizeof(type), data, size);

    esp_err_t result = esp_now_send(NULL, holder, sizeof(type) + size);

    if(result)
        return TX_FAIL;

    return TX_SUCCESS;
}



rx_status_t RadioNowHandler::CheckForPacket(packet_type_t* last_got)
{
    if(xQueueReceive(gotten_data_holder, raw_queue_dump, 0) == pdTRUE)
    {

        //get the type of data we have
        packet_type_t type = *(packet_type_t*)raw_queue_dump;

        switch(type)
        {
            default:
            case PACKET_NULL:
                Serial.println("Invalid Packet Type");
                break;
            case PACKET_CONTROL_VALUES:
                last_control_packet = *(remote_control_packet_t*)(raw_queue_dump + sizeof(packet_type_t));
                //Serial.println("Controller Packet");
                break;
            case PACKET_RESPONSE_VALUES:
                lask_ack_packet = *(remote_ack_packet_t*)(raw_queue_dump + sizeof(packet_type_t));
                //Serial.println("Response Packet");
                break;
            case PACKET_INTEGER:
            {
                unsigned int data = *(unsigned int*)(raw_queue_dump + sizeof(packet_type_t));
                Serial.printf("Integer Packet: %d\n", data);
                break;
            }
            case PACKET_ARRAY:
                Serial.println("Array Packet");
                break;
        }

        return RX_SUCCESS;

    }

    return RX_QUEUE_EMPTY;
}


remote_control_packet_t RadioNowHandler::GetLastControlPacket()
{return last_control_packet;}
remote_ack_packet_t RadioNowHandler::GetLastAckPacket()
{return lask_ack_packet;}

uint64_t RadioNowHandler::GetDeltaTime()
{
    //get time difference
    //uint64_t delta_time = deltaTime(millis(), rec_time);
    return deltaTime(millis(), rec_time);
}







