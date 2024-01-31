//Configuration
//this program was built using PlatformIO.
//with some tweaks, it COULD be built using the arduino IDE enviroment. All libraries used in this project are arduino


//this is not really needed, but I have it here for my personal reference. the arduino IDE doesn't like it here, but platformIO doesn't mind
//#ifndef USING_STM32
// Note: this needs to match with the PIN_MAP array in board.cpp
//STM32 pin mapping
//enum {
//    PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13,PA14,PA15,
//	PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13,PB14,PB15,
//	PC13, PC14,PC15
//};
//#endif


// Addresses for this node. CHANGE THESE FOR EACH NODE!

#define NETWORKID     0   // Must be the same for all nodes
#define MYNODEID      2   // My node ID
#define TONODEID      1   // Destination node ID


//#define FREQUENCY   RF69_433MHZ
#define FREQUENCY     RF69_915MHZ
#define IS_HIGH_POWER true


//pins for the radio
#define SLAVE_PIN PA4
#define IRQ_PIN PB4



#define ENCRYPT       true // Set to "true" to use encryption
#define ENCRYPTKEY    "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes



// Use ACKnowledge when sending messages (or not):
#define USEACK        true // Request ACKs or not

//for processing and sending out remote control channel data
#define CHANNEL_COUNT 12
#define USE_INVERTED_PPM true
