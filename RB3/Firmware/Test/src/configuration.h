//Configuration
#pragma once


////////RADIO SETTINGS////////

//espnow
#define NETWORKID           4   // Must be the same for all nodes
//custom MAC addresses
#define PEER_ADDRESS {0x30, 0xAE, 0xA4, 0x07, 0x0D, 0x64}
#define MY_ADDRESS {0xA0, 0x0E, 0x04, 0x0F, 0xFD, 0x64}


#define ENCRYPT       false // Set to "true" to use encryption
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



//something
#define KAKAJAMANGA

//for the STM32, random pins are used for testing: this part of the code will be used on the ESP32


//no radio transmission for 1 second, robot shuts off
#define KEEPALIVE_TIMEOUT_MS 1000

//slope mapping based on experimetnal data and resistor values (TODO: make this parametric)
#define VOLTMETER_SLOPE 0.0029
#define VOLTMETER_OFFSET 0.4129

//h-bridge operation:
//HI-HI is break
//LO-LO is no connection
//HI-LO is dir 1
//LO-HI is dir 2


//DRIVEBASE PINS
#define MOTOR_1A_PIN 0
#define MOTOR_1B_PIN 1
#define MOTOR_2A_PIN 2
#define MOTOR_2B_PIN 3

#define MOTOR_SLEEP_PIN 4


//WEAPON PINS
#define ESC_PIN 8

//ENCODER PINS
#define MOTOR_1_ENCA 10
#define MOTOR_1_ENCB 7
#define MOTOR_2_ENCA 6
#define MOTOR_2_ENCB 5




//////////////control surface mapping
//these are not directly used by the transmitter, but are the indexes of the transmitter packet that we use to check for this stuff

#define VERTICAL_LEFT 3
#define HORIZONTAL_LEFT 4
#define VERTICAL_RIGHT 2
#define HORIZONTAL_RIGHT 1
#define PADDLE_LEFT 5
#define PADDLE_RIGHT 6
#define TUNER_LEFT 7
#define TUNER_RIGHT 8

#define DIG_SA 1
#define DIG_POI 2
#define DIG_SC 3
#define DIG_FS 4
#define DIG_ATT 5
#define DIG_SF 6
#define DIG_DRRUD 7
#define DIG_SH 8


////////////////////normalization

//the max speed of the motor in both directions
#define MOTOR_RPM_MAX 600


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



