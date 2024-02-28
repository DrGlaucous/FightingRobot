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
#ifdef USING_STM32_BP
    #define SLAVE_PIN PA4
    #define IRQ_PIN PB4
#elif USING_ESP32
    #define SLAVE_PIN 26
    #define IRQ_PIN 35
#endif



#define ENCRYPT       true // Set to "true" to use encryption
#define ENCRYPTKEY    "HOTCRYSTALBABESS" //"TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes

// Use ACKnowledge when sending messages (or not):
#define USEACK        true // Request ACKs or not


////////BEHAVIOR SETTINGS////////

//what end of the stick the firmware is on
#define IS_CONTROLLER


////////////////PPM SETTINGS////////////////

//for processing and sending out remote control channel data

//how many analog channels we have
#define ANALOG_CHANNEL_CNT 8
//how many grouped digital channels we have (2 digital inputs per channel, due to how futaba remotes work)
#define DIGITAL_CHANNEL_CNT 4

#define CHANNEL_COUNT ANALOG_CHANNEL_CNT + DIGITAL_CHANNEL_CNT

//the range of the normalized analog values (mapped between [0,this], exclusive? or --inclusive?)
//adding detune will not affect these values
#define NORMAL_MIN 0
#define NORMAL_MAX 512


//PPM config stuff is only really needed for the transmitter
#ifdef IS_CONTROLLER

    //////NORMALIZATION CONSTANTS

    //min and max values read from PPM (WITHOUT DETUNE)
    #define ANALOG_MIN_LIM 550
    #define ANALOG_MAX_LIM 970


    //+- this ammount when calculating digital sums (realtive to raw PPM input)
    #define NORMAL_NOISE_ERR 5
    //+- this ammount when the aux. switch is flipped (main switches use (ANALOG_MAX_LIM - ANALOG_MIN_LIM)/2)
    #define DETUNE_VAL_SHIFT 64


    //////PPM LIBRARY CONFIG CONSTANTS
    //trigger on falling edge instead of rising edge (for futaba's weird 12 channel format)
    #define PPM_IS_INVERTED true

#ifdef USING_STM32_BP
    #define PPM_INTURRUPT_PIN PA0
#elif USING_ESP32
    #define PPM_INTURRUPT_PIN 27
#endif
    #define PPM_CHANNEL_COUNT CHANNEL_COUNT
    #define PPM_BLANK_TIME 5000
    #define PPM_MAX_WAIT_VALUE 1100
    #define PPM_MIN_WAIT_VALUE 400

    //default 8 channel
    //#define PPM_BLANK_TIME 2100
    //#define PPM_MAX_WAIT_VALUE 2000
    //#define PPM_MIN_WAIT_VALUE 1000
    //#define PPM_IS_INVERTED false

#else
    //something
    #define KAKAJAMANGA

    //for the STM32, random pins are used for testing: this part of the code will be used on the ESP32


    //no radio transmission for 1 second, robot shuts off
    #define KEEPALIVE_TIMEOUT_MS 1000

    //slope mapping based on experimetnal data and resistor values (TODO: make this parametric)
    #define VOLTMETER_SLOPE 0.0126 + 2.25

    //h-bridge operation:
    //HI-HI is break
    //LO-LO is no connection
    //HI-LO is dir 1
    //LO-HI is dir 2

#ifdef USING_STM32_BP
    //DRIVEBASE PINS
    #define MOTOR_1A_PIN PB0
    #define MOTOR_1B_PIN PB0

    #define MOTOR_2A_PIN PB0
    #define MOTOR_2B_PIN PB0

    #define MOTOR_3A_PIN PB0
    #define MOTOR_3B_PIN PB0

    #define MOTOR_SLEEP_PIN PB0


    //WEAPON PINS
    #define SERVO_1_PIN PB14
    #define SERVO_2_PIN PB15
    #define ESC_PIN PA8

#elif USING_ESP32
    //DRIVEBASE PINS
    #define MOTOR_1A_PIN 16
    #define MOTOR_1B_PIN 17

    #define MOTOR_2A_PIN 18
    #define MOTOR_2B_PIN 5

    #define MOTOR_3A_PIN 22
    #define MOTOR_3B_PIN 19

    #define MOTOR_SLEEP_PIN 23


    //WEAPON PINS
    #define SERVO_1_PIN 2
    #define SERVO_2_PIN 0
    #define ESC_PIN 4

    //OTHER
    #define VOLTMETER_PIN 34

#endif

    //control surface mapping
    
    //analog
    #define X_IN 0
    #define Y_IN 1
    #define TURN_IN 2
    #define SERVO_IN 3
    #define ESC_IN 4
    #define SERVO_MIN_IN 7
    #define SERVO_MAX_IN 8
    
    //digital
    #define FLIPOVER_IN 0
    #define ESC_REVERSE_IN 1
    #define TWO_MODE_IN 2
    #define TWO_SELECT_IN 3

    //normalization

    //analog pins go from 0-255, so this must be within that range
    #define XY_RADIUS 0xFF

    #define SERVO_MIN 0
    #define SERVO_MAX 180
    //range of motion on one side (depricated in favor of making this adjustable using the knobs on the remote)
    //#define SERVO_RANGE 100

    //number of magnets in the motor (for erpm measurements, only used in dshot mode)
    #define MOTOR_POLE_COUNT 14

#ifdef USING_STM32_BP
    //PWM speeds
    #define ESC_SPEED_MIDDLE 0 //'0' in 3D mode
    //full speed in 3D mode
    #define ESC_SPEED_MIN 0
    #define ESC_SPEED_MAX 180
#elif USING_ESP32
    //dshot speeds (better than PWM, but only works with the dshot library)
    #define ESC_SPEED_MIDDLE 1024 //'0' in 3D mode
    //full speed in 3D mode
    #define ESC_SPEED_MIN 48
    #define ESC_SPEED_MAX 2048
#endif






#endif

