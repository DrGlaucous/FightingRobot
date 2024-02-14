//gathers raw input from the control surface in the form of PPM
//also takes the PPM values and normalizes them

#pragma once

#include <Arduino.h>
#include <PPMReader.h>

#include "configuration.h"

//how many reads to cache for outlier removal (bigger means slower and smoother signals)
#define AVERAGE_POOL_CNT 3
//how far off an outlier should be before it is "corrected"
#define OUTLIER_THRESH 40

//the range of the normalized analog values (mapped between [0,this), exclusive? or inclusive?)
#define NORMAL_MIN 0
#define NORMAL_MAX 512

//+- this ammount when calculating digital sums
#define NORMAL_NOISE_ERR 10

//note: futaba's 12 channel protocol sends time-matched pairs of low then high signals.
//We could catch either just one or both together per channel. It doesn't matter




//all the variables needed to initialize the PPM library (for actually getting data from the control surface)
typedef struct ppm_config_s
{
    //times in microseconds
    bool is_falling = true; //trigger on the falling edge
    unsigned int blank_time = 5000; //minimum time between ppm signals
    unsigned int max_channel_value = 1100; //the longest a ppm signal will be
    unsigned int min_channel_value = 400; //the shortest a ppm signal will be

} ppm_config_t;





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

    //private functions

    //smooth the arrays above
    void ProcessOutliers();

    //put raw PPM into channel_array
    void GetControlSurface(uint16_t* channel_array, uint16_t array_len);

    //turn raw delay value into descrete switch positions
    void ParseSwitchSums(uint16_t raw_data, channel_digital_t *profile);


};








