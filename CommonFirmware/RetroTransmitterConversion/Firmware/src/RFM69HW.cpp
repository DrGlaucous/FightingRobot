#include "RFM69HW.h"


//these dang things keep this from being header-only.
RadioHandler* RadioHandler::instance = NULL;
void (*RadioHandler::custom_callback)() = NULL;

RadioHandler::RadioHandler() {
    instance = this;
    custom_callback = NULL;
}

RadioHandler::~RadioHandler() {
    if(radio != NULL) {
        delete radio;
        radio = NULL;
    }
    instance = NULL;
    custom_callback = NULL;
}

void RadioHandler::getCallback() {

    if(instance != NULL) {


        //note: this method causes HANGING!
        instance->getCallbackThis();
        

        if(custom_callback) {
            custom_callback();
        }

    }
}

void RadioHandler::getCallbackThis() {
    //process packet

    //if(radio->receiveDone()) {
    if(true) {


        last_RSSI = radio->RSSI;

        uint8_t data_length = radio->DATALEN;
        
        if(data_length > 0) {

            uint8_t * data_ptr = radio->DATA;
            last_packet_type = (ptype)(*data_ptr);

            //move read cursor forward
            data_ptr += sizeof(ptype);

            //copy packet values into storage
            memcpy(&last_packet, data_ptr, sizeof(Packet));

            //flag as fresh
            unread_packet = true;
        }
        if(radio->ACKRequested()) {
            radio->sendACK();
        }

    }
}

void RadioHandler::begin(
    SPIClass* spi,
    uint8_t slave_select_pin,
    uint8_t reset_pin,
    uint8_t rec_callback_pin,
    uint8_t my_id,
    uint8_t network_id,
    const char* encrypt_key
) {
    
    //ensure the module is reset before starting
    pinMode(reset_pin, OUTPUT);
    digitalWrite(reset_pin, 1);
    delay(1);
    digitalWrite(reset_pin, 0);
    delay(5);


    //SPI.begin(SCK, MISO, MOSI, NSS);
    radio = new RFM69(slave_select_pin, rec_callback_pin, true, spi);
    
    radio->initialize(RF69_915MHZ, my_id, network_id);
    radio->setHighPower();

    //turn on encryption if desired:
    if (encrypt_key)
        radio->encrypt(encrypt_key);
    
    //runs when we get data
    //attachInterrupt(rec_callback_pin, getCallback, HIGH);
    radio->setIsrCallback(getCallback);

}

void RadioHandler::setCustomRecCallback(void (*callback)()) {
    custom_callback = callback;
}

bool RadioHandler::sendPacket(Packet* packet, ptype packet_type, uint16_t address, bool use_ack) {

        char send_buffer[sizeof(Packet) + sizeof(ptype)] = {};
        char* sb_cursor = send_buffer;

        *sb_cursor = packet_type;
        sb_cursor += sizeof(ptype);
        memcpy(sb_cursor, packet, sizeof(Packet));

        if(use_ack) {
            return radio->sendWithRetry(address, send_buffer, sizeof(send_buffer));
        } else {
            radio->send(address, send_buffer, sizeof(send_buffer));
            return true; //assume transmission was a success
        }
    }

bool RadioHandler::checkForPackets(Packet& packet_clone, ptype& packet_type) {
    packet_clone = last_packet;
    packet_type = last_packet_type;

    bool is_new_data = unread_packet;
    unread_packet = false;
    return is_new_data;


}

