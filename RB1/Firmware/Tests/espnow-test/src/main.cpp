
#include <Arduino.h>

#include <esp_now.h>
#include <esp_wifi.h>
#include <Wifi.h>

#include "configuration.h"
#include "TimerTick.h"





//due to the use of the bleeding edge idf, I can't encrypt this...
#define ENCRYPT       false // Set to "true" to use encryption
//#define ENCRYPTKEY_S {0xA0,0xA0,0xFF,0x00,0xFF,0xA0,0xA0,0xA0,0xFF,0x45,0xA0,0xA0,0x26,0x20,0x43,0xA0}
#define ENCRYPTKEY_S    "HOTCRYSTALBABESS"
#define ENCRYPTKEY_P    "HOTCRYSTALBABESS" //"TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes


static const char* PMK_KEY_STR = "HOTCRYSTALBABESS";
static const char* LMK_KEY_STR = "HOTCRYSTALBABESS";


//custom MAC addresses
const uint8_t sender_addr[] = {0x30, 0xAE, 0xA4, 0x07, 0x0D, 0x64};
const uint8_t rec_addr[] = {0xA0, 0x0E, 0x04, 0x0F, 0xFD, 0x64};


const uint8_t lmk[] = ENCRYPTKEY_S;

//setup structs
esp_now_peer_info_t sender_info;
esp_now_peer_info_t rec_info;

TimerHandler gTimer; 



// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
//   if (status ==0){
//     success = "Delivery Success :)";
//   }
//   else{
//     success = "Delivery Fail :(";
//   }

}

// Callback when data is received
void OnDataRecv(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len) {
  Serial.print("Bytes received: ");
  Serial.println(data_len); 
}






void setup()
{

    Serial.begin(115200);

    // Register peers
    memcpy(sender_info.peer_addr, sender_addr, 6);
    sender_info.channel = 0;  
    sender_info.encrypt = ENCRYPT;

    memcpy(rec_info.peer_addr, rec_addr, 6);
    rec_info.channel = 0;  
    rec_info.encrypt = ENCRYPT;

    //memcpy(sender_info.lmk, lmk, sizeof(lmk));
    //memcpy(rec_info.lmk, lmk, sizeof(lmk));

    for (uint8_t i = 0; i < 16; i++) {
        sender_info.lmk[i] = LMK_KEY_STR[i];
        rec_info.lmk[i] = LMK_KEY_STR[i];
    }

    //set up radio

    WiFi.mode(WIFI_STA);

    //note: for some reason, WiFi.macAddress() does not give the proper mac address, but from experimentation, it changing it *does* seem to work...

    Serial.print("[OLD] ESP32 Board MAC Address:  ");
    Serial.println(WiFi.macAddress());

    //Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    //tx callback
    esp_now_register_send_cb(OnDataSent);


    if(ENCRYPT) {
       auto err = esp_now_set_pmk((uint8_t*)PMK_KEY_STR);
       Serial.printf("esp_now_set_pmk: %d\n", err);
    }

    esp_now_peer_num_t num;
    esp_now_get_peer_num(&num);
    Serial.printf("Peer ct: %d, %d\n", num.total_num, num.encrypt_num);
     

    //set up self and peer
#ifdef IS_SENDER
    esp_wifi_set_mac(WIFI_IF_STA, sender_addr);

    esp_err_t peer_result = esp_now_add_peer(&rec_info);
    if (peer_result != ESP_OK){
        Serial.printf("Failed to add peer: %d: ", peer_result);

        switch(peer_result) {
            case ESP_ERR_ESPNOW_NOT_INIT: {
                Serial.printf("ESP_ERR_ESPNOW_NOT_INIT\n");
                break;
            }
            case ESP_ERR_ESPNOW_ARG: {
                Serial.printf("ESP_ERR_ESPNOW_ARG\n");
                break;
            }
            case ESP_ERR_ESPNOW_FULL: {
                Serial.printf("ESP_ERR_ESPNOW_FULL\n");
                break;
            }
            case ESP_ERR_ESPNOW_NO_MEM: {
                Serial.printf("ESP_ERR_ESPNOW_NO_MEM\n");
                break;
            }
            case ESP_ERR_ESPNOW_EXIST: {
                Serial.printf("ESP_ERR_ESPNOW_EXIST\n");
                break;
            }
        }

        esp_now_get_peer_num(&num);
        Serial.printf("Peer ct: %d, %d\n", num.total_num, num.encrypt_num);
        return;
    }


#else
    esp_wifi_set_mac(WIFI_IF_STA, rec_addr);

    esp_err_t peer_result = esp_now_add_peer(&sender_info);
    if (peer_result != ESP_OK){
        Serial.printf("Failed to add peer: %d: ", peer_result);

        switch(peer_result) {
            case ESP_ERR_ESPNOW_NOT_INIT: {
                Serial.printf("ESP_ERR_ESPNOW_NOT_INIT\n");
                break;
            }
            case ESP_ERR_ESPNOW_ARG: {
                Serial.printf("ESP_ERR_ESPNOW_ARG\n");
                break;
            }
            case ESP_ERR_ESPNOW_FULL: {
                Serial.printf("ESP_ERR_ESPNOW_FULL\n");
                break;
            }
            case ESP_ERR_ESPNOW_NO_MEM: {
                Serial.printf("ESP_ERR_ESPNOW_NO_MEM\n");
                break;
            }
            case ESP_ERR_ESPNOW_EXIST: {
                Serial.printf("ESP_ERR_ESPNOW_EXIST\n");
                break;
            }
        }

        esp_now_get_peer_num(&num);
        Serial.printf("Peer ct: %d, %d\n", num.total_num, num.encrypt_num);

        return;
    }

#endif


    esp_now_get_peer_num(&num);
    Serial.printf("Peer ct: %d, %d\n", num.total_num, num.encrypt_num);


    esp_now_register_recv_cb(OnDataRecv);

    Serial.print("[NEW] ESP32 Board MAC Address:  ");
    Serial.println(WiFi.macAddress());
}


void loop()
{

    gTimer.update();

    static unsigned long delta_t = 0;
    static unsigned int l_state = 0;
    if(gTimer.DeltaTimeMillis(&delta_t, 500))
    {
        ++l_state;

        esp_err_t result = esp_now_send(NULL, (uint8_t *) &l_state, sizeof(l_state));

        Serial.printf("beat %d\n", l_state);

    }


}





