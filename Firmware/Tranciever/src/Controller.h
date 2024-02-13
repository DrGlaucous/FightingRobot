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

//the range of the normalized analog values (mapped between [0,this), exclusive)
#define NORMAL_MAX 512

//note: futaba's 12 channel protocol sends time-matched pairs of low then high signals.
//We could catch either just one or both together per channel. It doesn't matter




//all the variables needed to initialize the PPM library
typedef struct ppm_config_s
{
    //times in microseconds
    bool is_falling = true; //trigger on the falling edge
    unsigned int blank_time = 5000; //minimum time between ppm signals
    unsigned int max_channel_value = 1100; //the longest a ppm signal will be
    unsigned int min_channel_value = 400; //the shortest a ppm signal will be

} ppm_config_t;

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

    //the changes in PPM delay expected to be inflicted by each switch
    unsigned int value_shift_main;
    unsigned int value_shift_second;

} channel_digital_t;

class ControllerHandler
{

    public:
    ControllerHandler();
    ~ControllerHandler();

    //runs both the PPM getter and the normalizer for any new inputs
    void update();


    //using the initial config, normalizes an array of PPM values
    void NormalizePPM(uint16_t* channels, uint16_t len);

    //copies the PPM from the class to the variable passed to the function
    void GetRawPPM(uint16_t* channels, uint16_t len);


    private:

    //cache the last X reads for outlier averaging
    uint16_t raw_channels[AVERAGE_POOL_CNT][CHANNEL_COUNT] = {};
    uint8_t recent_index;

    //array holding the smoothed ppm values from the controller
    uint16_t processed_channels[CHANNEL_COUNT] = {};


};








