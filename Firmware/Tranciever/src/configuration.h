//Configuration



////////RADIO SETTINGS////////

// Addresses for this node. CHANGE THESE FOR EACH NODE!

#define NETWORKID           0   // Must be the same for all nodes
#define RECEIVERNODEID      1   //node the transmitter lives on
#define TRANSMITTERNODEID   2   //node the robot lives on


//#define FREQUENCY   RF69_433MHZ
#define FREQUENCY     RF69_915MHZ
#define IS_HIGH_POWER true

//pins for the radio
#define SLAVE_PIN PA4
#define IRQ_PIN PB4

#define ENCRYPT       true // Set to "true" to use encryption
#define ENCRYPTKEY    "HOTCRYSTALBABESS" //"TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes

// Use ACKnowledge when sending messages (or not):
#define USEACK        true // Request ACKs or not



////////BEHAVIOR SETTINGS////////

//for processing and sending out remote control channel data
#define CHANNEL_COUNT 12

//what end of the stick the firmware is on
#define IS_CONTROLLER


//settings specific to the transmitter

#ifdef IS_CONTROLLER
    //trigger on falling edge instead of rising edge (for futaba's weird 12 channel format)
    #define PPM_IS_INVERTED true

    #define PPM_INTURRUPT_PIN PA0
    #define PPM_CHANNEL_COUNT CHANNEL_COUNT
    #define PPM_BLANK_TIME 5000
    #define PPM_MAX_WAIT_VALUE 1100
    #define PPM_MIN_WAIT_VALUE 400
#else
    //something
    #define KAKAJAMANGA
#endif

