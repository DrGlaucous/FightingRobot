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

//what end of the stick the firmware is on
//#define IS_CONTROLLER


////////////////PPM SETTINGS////////////////

//for processing and sending out remote control channel data

//how many analog channels we have
#define ANALOG_CHANNEL_CNT 8
//how many grouped digital channels we have (2 digital inputs per channel, due to how futaba remotes work)
#define DIGITAL_CHANNEL_CNT 4

#define CHANNEL_COUNT ANALOG_CHANNEL_CNT + DIGITAL_CHANNEL_CNT

//the range of the normalized analog values (mapped between [0,this), exclusive? or inclusive?)
#define NORMAL_MIN 0
#define NORMAL_MAX 512


//PPM config stuff is only really needed for the transmitter
//#ifdef IS_CONTROLLER

    //////NORMALIZATION CONSTANTS

    //min and max values read from PPM (WITHOUT DETUNE)
    #define ANALOG_MIN_LIM 550
    #define ANALOG_MAX_LIM 970

    //outlier removal is depricated in lieu of the updated PPM reading library
    //how many reads to cache for outlier removal (bigger means slower and smoother signals)
    //#define AVERAGE_POOL_CNT 3
    //how far off an outlier should be before it is "corrected"
    //#define OUTLIER_THRESH 40

    //+- this ammount when calculating digital sums
    #define NORMAL_NOISE_ERR 10


    //////PPM LIBRARY CONFIG CONSTANTS
    //trigger on falling edge instead of rising edge (for futaba's weird 12 channel format)
    #define PPM_IS_INVERTED true

    #define PPM_INTURRUPT_PIN PA0
    #define PPM_CHANNEL_COUNT CHANNEL_COUNT
    #define PPM_BLANK_TIME 5000
    #define PPM_MAX_WAIT_VALUE 1100
    #define PPM_MIN_WAIT_VALUE 400

    //default 8 channel
    //#define PPM_BLANK_TIME 2100
    //#define PPM_MAX_WAIT_VALUE 2000
    //#define PPM_MIN_WAIT_VALUE 1000
    //#define PPM_IS_INVERTED false

//#else
    //something
    #define KAKAJAMANGA
//#endif

