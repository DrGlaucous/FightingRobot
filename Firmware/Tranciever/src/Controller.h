//gathers raw input from the control surface in the form of PPM
//also takes the PPM values and normalizes them
#pragma once

#include <Arduino.h>
#include <PPMReader.h>

#include "configuration.h"



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
//some levers detune by +-30% (full range of 60%)
//970 + detune = 1033, 970 - detune = 906

//complete range is 420ms, centerpoint is 210
//30% detune is 64ms, x2 is 128 (calculated: 126)



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





//all of these structs take the smooth value that was computed using the groupings in the main class



//for things like sliders and control sticks
typedef struct channel_analog_s
{
    uint16_t value_normalized;

    //values to normalize input between (values outside will be truncated)
    unsigned int min_raw_val;
    unsigned int max_raw_val;

} channel_analog_t;
//for things like buttons and switches (usually 2 switches share a single channel)
typedef struct channel_digital_s
{
    //switched between 0, 1, 2
    byte switch_main;
    byte switch_second;

    //the changes in normalized delay expected to be inflicted by each switch
    unsigned int value_shift_main;
    unsigned int value_shift_second;

    //holds limits and value for normalizing the switch to begin with
    channel_analog_t normalized_in;

} channel_digital_t;




class ControllerHandler
{

    public:
    ControllerHandler();
    ~ControllerHandler();

    //runs both the PPM getter and the normalizer for any new inputs
    void update();


    //using the initial config, normalizes an array of PPM values
    void NormalizePPM(uint16_t raw_data, channel_analog_t* normalized_data);

    //copies the PPM from the class to the variable passed to the function
    void GetRawPPM(uint16_t* channels, uint16_t len);


    private:

    //pointer to PPM
    PPMReader* ppm;

    //cache the last X reads for outlier averaging
    uint16_t raw_channels[AVERAGE_POOL_CNT][CHANNEL_COUNT] = {};
    
    //goes through the list of AVERAGE_POOL_CNT so we don't need to shift values around
    uint8_t this_index = {};
    uint8_t last_index = {}; //it's OK if these are initalized to the same value

    //array holding the smoothed ppm values from the controller
    uint16_t processed_channels[CHANNEL_COUNT] = {};


    //holds all the data we get from the PPM, analog channels are indexed first when collecting data
    channel_analog_t analog_channels[ANALOG_CHANNEL_CNT] = {};
    channel_digital_t digital_channels[DIGITAL_CHANNEL_CNT] = {};


    //private functions

    //smooth the arrays above =note= we fixed the problems with the PPM reader, this is depricated now
    void ProcessOutliers();

    //put raw PPM into channel_array
    void GetControlSurface(uint16_t* channel_array, uint16_t array_len);

    //turn raw delay value into descrete switch positions
    void ParseSwitchSums(uint16_t raw_data, channel_digital_t *profile);

    //test
    void PrintRawChannels(uint16_t* channel_array, uint16_t array_len);




};

