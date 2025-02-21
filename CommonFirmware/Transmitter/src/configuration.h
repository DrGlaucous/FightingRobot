//Configuration



////////RADIO SETTINGS////////

//espnow
#define NETWORKID           4   // Must be the same for all nodes
//custom MAC addresses
#define SENDER_ADDR {0x30, 0xAE, 0xA4, 0x07, 0x0D, 0x64}
#define REC_ADDR {0xA0, 0x0E, 0x04, 0x0F, 0xFD, 0x64}


#define ENCRYPT       true // Set to "true" to use encryption
#define ENCRYPTKEY_S {0xA0,0xA0,0xFF,0x00,0xFF,0xA0,0xA0,0xA0,0xFF,0x45,0xA0,0xA0,0x26,0x20,0x43,0xA0}
#define ENCRYPTKEY_P    "HOTCRYSTALBABESS" //"TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes


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


#define PPM_INTURRUPT_PIN 16
#define PPM_CHANNEL_COUNT CHANNEL_COUNT
//was in millis, now in nanos
#define PPM_BLANK_TIME 2000000 //5000
#define PPM_MIN_WAIT_VALUE 500000 //500000

//#define PPM_MAX_WAIT_VALUE 1100


//default 8 channel
//#define PPM_BLANK_TIME 2100
//#define PPM_MAX_WAIT_VALUE 2000
//#define PPM_MIN_WAIT_VALUE 1000
//#define PPM_IS_INVERTED false


//control surface mapping

//analog
#define X_IN 0
#define Y_IN 1
#define TURN_IN 3
#define SERVO_IN 2
#define ESC_IN 4
#define SERVO_MIN_IN 5
#define TURN_RAMP_TUNE 6
#define MOTOR_RAMP_TUNE 7

//digital
#define FLIPOVER_IN 1//5
#define ESC_REVERSE_IN 0//4
#define TWO_MODE_IN 7
#define TWO_SELECT_IN 6
#define MASTER_ENABLE 2


//normalization

//analog pins go from 0-255, so this must be within that range
#define XY_RADIUS 0xFF

//analog servos have slightly different values that are mapped from the normalized range
#define SERVO_1_MS_MIN 500//600 //500 //604
#define SERVO_1_MS_MAX 2500//2200 //2500 //2200

#define SERVO_2_MS_MIN 500//660 //500 //660
#define SERVO_2_MS_MAX 2500//2100 //2500 //2100
//normalized range, shared between both servos
#define SERVO_MIN 0
#define SERVO_MAX 512


//number of magnets in the motor (for erpm measurements, only used in dshot mode)
#define MOTOR_POLE_COUNT 14


//backwards is 48-1047,
//forwards is 1048-2047
//dshot speeds (better than PWM, but only works with the dshot library)
//full speed in 3D mode
#define ESC_SPEED_MIN_R 48
#define ESC_SPEED_MAX_R 1047
#define ESC_SPEED_MIN_F 1048
#define ESC_SPEED_MAX_F 2047



