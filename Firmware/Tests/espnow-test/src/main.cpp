
#include <Arduino.h>

#include <esp_now.h>
#include <esp_wifi.h>
#include <Wifi.h>

#include "configuration.h"
#include "TimerTick.h"


//#define IS_SENDER

#ifdef IS_SENDER

#else

#endif

//custom MAC addresses
uint8_t sender_addr[] = {0x30, 0xAE, 0xA4, 0x07, 0x0D, 0x64};
uint8_t rec_addr[] = {0xA0, 0x0E, 0x04, 0x0F, 0xFD, 0x64};

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
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  Serial.print("Bytes received: ");
  Serial.println(len);
}






void setup()
{

    Serial.begin(115200);
    pinMode(MOTOR_3A_PIN, OUTPUT);
    pinMode(MOTOR_3B_PIN, OUTPUT);

    pinMode(MOTOR_SLEEP_PIN, OUTPUT);

    digitalWrite(MOTOR_3A_PIN, true);
    digitalWrite(MOTOR_3B_PIN, false);

    digitalWrite(MOTOR_SLEEP_PIN, false);

    // Register peers
    memcpy(sender_info.peer_addr, sender_addr, 6);
    sender_info.channel = 0;  
    sender_info.encrypt = false;

    memcpy(rec_info.peer_addr, rec_addr, 6);
    rec_info.channel = 0;  
    rec_info.encrypt = false;


    //set up radio

    WiFi.mode(WIFI_STA);

    Serial.print("[OLD] ESP32 Board MAC Address:  ");
    Serial.println(WiFi.macAddress());

    //Init ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }

    //tx callback
    esp_now_register_send_cb(OnDataSent);


    //set up self and peer
#ifdef IS_SENDER
    esp_wifi_set_mac(WIFI_IF_STA, sender_addr);
       
    if (esp_now_add_peer(&rec_info) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }
#else
    esp_wifi_set_mac(WIFI_IF_STA, rec_addr);

    if (esp_now_add_peer(&sender_info) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }
#endif

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
        digitalWrite(MOTOR_3A_PIN, l_state % 2);
        ++l_state;

        esp_err_t result = esp_now_send(NULL, (uint8_t *) &l_state, sizeof(l_state));

        Serial.printf("beat %d\n", l_state);

    }


}





