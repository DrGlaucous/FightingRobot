


#include <RFM69.h>
//#include "configuration.h"

//number of control surfaces on the controller, can be overwritten
#ifndef DIGITAL_CHANNELS_CT
#define DIGITAL_CHANNELS_CT 4
#endif

#ifndef ANALOG_CHANNELS_CT
#define ANALOG_CHANNELS_CT 9
#endif



typedef uint8_t ptype;

//hard-programmed packet ID types
constexpr ptype CONTROL_TYPE = 0;
constexpr ptype RESPONSE_TYPE = 1;



//packet sent from the controller to the machine
#pragma pack(1) //force compiler to compact the struct
typedef struct ControllerPacket {
    uint8_t digital_switches[DIGITAL_CHANNELS_CT] = {};
    int16_t analog_channels[ANALOG_CHANNELS_CT] = {};
} ControllerPacket;


//packet sent from the bot to the controller
#pragma pack(1) 
typedef struct ResponsePacket {
    float_t battery_voltage = 0.0;
} ResponsePacket;


//shared packet interface
#pragma pack(1) 
typedef union Packet {
    ResponsePacket response;
    ControllerPacket control;
} Packet;


//note: is a singleton class
class RadioHandler {

private:

    //only one instance at a time
    static RadioHandler* instance;
    static void (*custom_callback)();

    //last rx data, there is no caching system.
    //Packet last_packet = {};
    //ptype last_packet_type = RESPONSE_TYPE;
    //bool unread_packet = false;
    int16_t last_RSSI = 0;


public:

    RFM69 *radio = NULL;
    
    RadioHandler();

    ~RadioHandler();


    //static callback function, run when we get a packet from the radio module.
    static void getCallback();

    //non-static callback function
    void getCallbackThis();

    //set up the RFM69 class, callbacks, and other housekeeping
    //encrypt_key is 16 bytes long and should be the same on all nodes.
    void begin(
        SPIClass* spi,
        uint8_t slave_select_pin,
        uint8_t reset_pin,
        uint8_t rec_callback_pin,
        uint8_t my_id,
        uint8_t network_id,
        const char* encrypt_key
    );

    //custom callback called outside this class
    void setCustomRecCallback(void (*callback)());

    //send a packet out to a device
    bool sendPacket(Packet* packet, ptype packet_type, uint16_t address, bool use_ack);

    //returns true if the packet is new and false if this method was called multiple times before the next rx
    bool checkForPackets(Packet& packet_clone, ptype& packet_type);




};






