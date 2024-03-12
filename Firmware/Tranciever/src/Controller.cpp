

#include <Arduino.h>
#include <PPMReader.h>

#include "Controller.h"
#include "configuration.h"
#include "TimerTick.h"

#ifdef IS_CONTROLLER

ControllerHandler::ControllerHandler()
{
    ppm = new PPMReader(PPM_INTURRUPT_PIN, CHANNEL_COUNT, PPM_IS_INVERTED);

    //setup PPM values for 12 channel mode
    ppm->blankTime = PPM_BLANK_TIME;//10000;
    ppm->maxChannelValue = PPM_MAX_WAIT_VALUE;
    ppm->minChannelValue = PPM_MIN_WAIT_VALUE;

}

ControllerHandler::~ControllerHandler()
{
    delete(ppm);
}

bool ControllerHandler::update()
{
    //get PPM from remote
    GetControlSurface(p_channels, PPM_CHANNEL_COUNT);
    

    //check if values are OOB (report error if so, this usually means the transmitter is being turned on)
    bool has_err = false;
    for(int i = 0; i < PPM_CHANNEL_COUNT; ++i)
    {
        if(p_channels[i] < PPM_MIN_WAIT_VALUE || p_channels[i] > PPM_MAX_WAIT_VALUE)
        {
            has_err = true;
        }
    }


    //test
    PrintRawChannels(p_channels, PPM_CHANNEL_COUNT);

    //normalize data for analog channels
    for(int i = 0; i < ANALOG_CHANNEL_CNT; ++i)
    {
        NormalizePPM(p_channels[i], &analog_channels[i]);
    }
    //process digital channels
    for(int i = 0; i < DIGITAL_CHANNEL_CNT; ++i)
    {
        ParseSwitchSums(p_channels[ANALOG_CHANNEL_CNT + i], &digital_channels[i]);
    }
    //test
    //PrintProcessedChannels();

    return has_err;

}

//return a combined list of all radio data
concatated_channels_t ControllerHandler::GetReadyPacket()
{
    concatated_channels_t output = {};
    for(int i = 0; i < ANALOG_CHANNEL_CNT; ++i)
        output.analog_channels[i] = analog_channels[i].value_normalized;

    for(int i = 0; i < DIGITAL_CHANNEL_CNT; ++i)
    {
        output.digital_channels[i * 2] = digital_channels[i].switch_main;
        output.digital_channels[i * 2 + 1] = digital_channels[i].switch_second;
    }
    return output;
}

//read PPM values from the controller and run processOutliers
void ControllerHandler::GetControlSurface(uint16_t* channel_array, uint16_t array_len)
{

    //read PPM into packet struct
    for (byte channel = 1; channel <= array_len; ++channel) {

        //note: ppm.latestValidChannelValue is base-1!
        channel_array[channel - 1] = ppm->latestValidChannelValue(channel, channel_array[channel - 1]);
        
        //test: report back serial values
        //Serial.printf("%d, \t", channel_array[channel - 1]);
    }
    //test
    //Serial.println();


}

//take a channel and normalize it according to the channel channel's config
void ControllerHandler::NormalizePPM(uint16_t raw_data, channel_analog_t* normalized_data)
{

    //truncate OOB
    if(raw_data < ANALOG_MIN_LIM)
        raw_data = ANALOG_MIN_LIM;
    else if(raw_data > ANALOG_MAX_LIM)
        raw_data = ANALOG_MAX_LIM;

    //normalize
    normalized_data->value_normalized = map(raw_data, ANALOG_MIN_LIM, ANALOG_MAX_LIM, NORMAL_MIN, NORMAL_MAX);

}
//take a channel and determine the two switch positions based on channel's config
void ControllerHandler::ParseSwitchSums(uint16_t raw_data, channel_digital_t *profile)
{

    /*
    //'0' state should be 256
    //NORMAL_MAX / 2

    //check for large state switch loations
    
    //9 states:

    //big switch: +-210
    //0 - 550
    //1 - 760
    //2 - 970

    //small switch: +-64
    //0 - -64
    //1 - 0
    //2 - 64


    //big state 0
    // MIN - DETUNE - NOISE < X < MIN - DETUNE + NOISE
    // X < MIN + (DETUNE + NOISE)
    // MIDPOINT = MIN

    //big state 1
    // (MAX-MIN)/2 - DETUNE - NOISE < X < (MAX-MIN)/2 + DETUNE + NOISE
    // cannot simplify
    // MIDPOINT = (MAX-MIN)/2

    //big state 2
    // MAX - DETUNE - NOISE < X < MAX + DETUNE + NOISE
    // MAX - DETUNE - NOISE < X
    // MIDPOINT = MAX

    //small state 0
    // X < MIDPOINT - NOISE

    //small state 1
    //else

    //small state 2
    // X > MIDPOINT + NOISE
    */

   //going off of config constants, not these anymore
   //unsigned int min_loc = (ANALOG_MAX_LIM - ANALOG_MIN_LIM)/2 - profile->value_shift_main;
   //unsigned int max_loc = min_loc + 2 * profile->value_shift_main;


    // find large switch position
    unsigned int midpoint = 0;
    if(raw_data < ANALOG_MIN_LIM + DETUNE_VAL_SHIFT + NORMAL_NOISE_ERR)
    {
        profile->switch_main = 0;
        midpoint = ANALOG_MIN_LIM;
    }
    else if(raw_data > ANALOG_MAX_LIM - DETUNE_VAL_SHIFT - NORMAL_NOISE_ERR)
    {
        profile->switch_main = 2;
        midpoint = ANALOG_MAX_LIM;
    }
    else
    {
        profile->switch_main = 1;
        midpoint = (ANALOG_MAX_LIM - ANALOG_MIN_LIM) / 2 + ANALOG_MIN_LIM;
    }


    //find small switch position
    if(raw_data < midpoint - NORMAL_NOISE_ERR)
    {
        profile->switch_second = 0;
    }
    else if(raw_data > midpoint + NORMAL_NOISE_ERR)
    {
        profile->switch_second = 2;
    }
    else
    {
        profile->switch_second = 1;
    }




}

//test: read back PPM vals
void ControllerHandler::PrintRawChannels(uint16_t* channel_array, uint16_t array_len)
{
    for (byte channel = 1; channel <= array_len; ++channel)
    {
        //test: report back serial values
        Serial.printf("%d\t", channel_array[channel - 1]);
    }
    //test
    Serial.println();
}
void ControllerHandler::PrintProcessedChannels()
{
    for(int i = 0; i < ANALOG_CHANNEL_CNT; ++i)
    {
        Serial.printf("Analog %d: Normal: %d\n", i, analog_channels[i].value_normalized);
    }

    for(int i = 0; i < DIGITAL_CHANNEL_CNT; ++i)
    {
        Serial.printf("Digital %d: SW 1: %d SW 2: %d\n", i, digital_channels[i].switch_main, digital_channels[i].switch_second);
    }
}




#endif