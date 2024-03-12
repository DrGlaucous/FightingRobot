#include <Arduino.h>

#include <esp_now.h>
#include <esp_wifi.h>
#include <Wifi.h>

#include "ESPRadio.h"
#include "configuration.h"

bool RadioHandlerExsists = false;
QueueHandle_t gotten_data_holder;

void PacketSentCallback(const uint8_t *mac_addr, esp_now_send_status_t status)
{
    //not needed for now
    Serial.println("Sent");
}
void PacketGotCallback(const uint8_t * mac, const uint8_t *incomingData, int len)
{
    Serial.println("Got");

    BaseType_t high_task_wakeup = pdFALSE;
    //xQueueSendFromISR(gotten_data_holder, incomingData, &high_task_wakeup);
    xQueueOverwriteFromISR(gotten_data_holder, incomingData, &high_task_wakeup);
}

RadioNowHandler::RadioNowHandler()
{
    if(RadioHandlerExsists)
    {
        //err
        return;
    }

    RadioHandlerExsists = true;

    if(gotten_data_holder != NULL)
    {
        //create the callback data queue if it's not already present
        gotten_data_holder = xQueueCreate(1, ESP_NOW_MAX_DATA_LEN);
    }
    //check for successful creation
    if(gotten_data_holder == NULL)
    {
        //err
    }


    //setup the radios
    WiFi.mode(WIFI_STA);

    //Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    esp_now_register_send_cb(PacketSentCallback);

#ifdef IS_CONTROLLER
    const uint8_t new_mac[] = SENDER_ADDR;
    esp_now_peer_info_t peer = {REC_ADDR, N_ENCRYPTKEY};
#else
    const uint8_t new_mac[] = REC_ADDR;
    esp_now_peer_info_t peer = {SENDER_ADDR, N_ENCRYPTKEY};
#endif

    peer.channel = NETWORKID;
    peer.encrypt = ENCRYPT;


    esp_wifi_set_mac(WIFI_IF_STA, new_mac);

    if (esp_now_add_peer(&peer) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }

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


tx_status_t RadioNowHandler::SendPacket(remote_control_packet_t packet)
{
    //enforce packettype
    packet.packettype = PACKET_CONTROL_VALUES;
    uint8_t size = sizeof(packet);

    esp_err_t result = esp_now_send(NULL, (uint8_t *) &packet, size);
    
    //failure
    if(result)
        return TX_FAIL;

    return TX_SUCCESS;

}



bool RadioNowHandler::CheckForPacket(packet_type_t* last_got)
{
    //if(xQueueReceive(gotten_data_holder, raw_queue_dump, 0) == pdTRUE)
    {
        return true;
    }

    return false;
}













