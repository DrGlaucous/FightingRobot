#include <RFM69.h>



// Addresses for this node. CHANGE THESE FOR EACH NODE!
//#define SENDER


#define NETWORKID 0 // Must be the same for all nodes

#ifdef SENDER
#define MYNODEID 1	// My node ID
#define TONODEID 2	// Destination node ID
#else
#define MYNODEID 2	// My node ID
#define TONODEID 1	// Destination node ID
#endif


//RFM69 frequency,
#define FREQUENCY RF69_915MHZ

// Use the same 16-byte key on all nodes
#define ENCRYPTKEY "HOTCRYSTALBABESS" //"TOPSECRETPASSWRD"

// Use ACKnowledge when sending messages (or not):
#define USEACK false // Request ACKs or not


//PIN DEFINITIONS

#ifdef USING_ESP32

//custom SPI pin definitions
#define PIN_SCK 1
#define PIN_MISO 2
#define PIN_MOSI 3
#define PIN_NSS 4
#define PIN_RESET 5

//RFM69hw isr pin
#define PIN_ISR 35


#elif USING_STM32

//the STM32 has a fixed SPI output, so we just set these.
#define PIN_ISR PB4
#define PIN_NSS PA4
#define PIN_RESET PB5

#endif












