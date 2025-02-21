//gathers raw input from the control surface in the form of PPM
//also takes the PPM values and normalizes them
#pragma once

#include <Arduino.h>

#include "configuration.h"
#include "PPMRMT.h"


//note: futaba's 12 channel protocol sends time-matched pairs of low then high signals.
//We could catch either just one or both together per channel. It doesn't matter
//with current settings, input values range from 970 to 550 (+- 2 or occasionally 3 for non-edge signals)
//(right/bottom) is 970, (left/top) is 550
//on back levers, up is 550, down is 970
//on front knobs, CCW is MAX (1033), CW is MIN (487) (extra due to 30% detune, more on this later)
//note: on non-lever elements, remove the self-detune (for correct normalization)

//760 is center
//when levers are UP, that mean's the're 'ON' (shorter PPM)


//on servo menu, top left is (-100, -100) [550,550], just like PC diplay coords
//bottom right is (100,100) [970,970]
//some levers detune by +-15% (full range of 30%)
//970 + detune = 1033, 970 - detune = 906

//complete range is 420ms, centerpoint is 210
//30% detune is 64ms, x2 is 128 (calculated: 126)


//normalization rules:

//analog channels will vary between 0 and 1024, period. It will not exceed these numbers
//digital values will vary from 0-2, depending on the switch position




//all the variables needed to initialize the PPM library (for actually getting data from the control surface)
//not used ATM
// typedef struct ppm_config_s
// {
//     //ppm config stuff
//     //times in microseconds
//     unsigned int blank_time = PPM_BLANK_TIME; //minimum time between ppm signals
//     unsigned int max_channel_value = PPM_MAX_WAIT_VALUE; //the longest a ppm signal will be
//     unsigned int min_channel_value = PPM_MIN_WAIT_VALUE; //the shortest a ppm signal will be
//     unsigned int inturrupt_pin = PPM_INTURRUPT_PIN; //pin to get delays from
//     bool inverted_ppm = PPM_IS_INVERTED; //trigger on the falling edge
//     //general control surface stuff
//     unsigned int channel_count = PPM_CHANNEL_COUNT; //number of channels
// } ppm_config_t;



//all of these structs take the raw PPM value

//for things like sliders and control sticks
typedef struct channel_analog_s
{
    uint16_t value_normalized;

    //values to normalize input between (values outside will be truncated)
    //currently defined constants
    //unsigned int min_raw_val;
    //unsigned int max_raw_val;

} channel_analog_t;
//for things like buttons and switches (usually 2 switches share a single channel)
typedef struct channel_digital_s
{
    //switched between 0, 1, 2
    byte switch_main;
    byte switch_second;

    //the changes in raw delay expected to be inflicted by each switch
    //currently defined constants
    //unsigned int value_shift_main;
    //unsigned int value_shift_second;

    //holds limits and value for normalizing the switch to begin with
    //channel_analog_t normalized_in;

} channel_digital_t;

//used to transmit a packet over the radio to the reciever
#pragma pack(1)
typedef struct concatated_channels_s
{
    uint16_t analog_channels[ANALOG_CHANNEL_CNT] = {};
    //2 switches per each digital channel
    uint8_t digital_channels[DIGITAL_CHANNEL_CNT * 2] = {};
}concatated_channels_t;
