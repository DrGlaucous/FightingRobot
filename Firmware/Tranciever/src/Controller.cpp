

#include <Arduino.h>
#include <PPMReader.h>

#include "Controller.h"
#include "configuration.h"
#include "TimerTick.h"


ControllerHandler::ControllerHandler()
{
    ppm = new PPMReader(PPM_INTURRUPT_PIN, PPM_CHANNEL_COUNT, PPM_IS_INVERTED);

    //setup PPM values for 12 channel mode
    ppm->blankTime = PPM_BLANK_TIME;//10000;
    ppm->maxChannelValue = PPM_MAX_WAIT_VALUE;
    ppm->minChannelValue = PPM_MIN_WAIT_VALUE;

}

ControllerHandler::~ControllerHandler()
{
    delete(ppm);
}


void ControllerHandler::update()
{
    //get PPM from remote
    GetControlSurface(raw_channels[this_index], PPM_CHANNEL_COUNT);
    
    
    //process the outliers
    //ProcessOutliers();

    //roll over the index tickers
    last_index = this_index;
    if(++this_index >= AVERAGE_POOL_CNT)
        this_index = 0;

}


//handle recieved outliers
void ControllerHandler::ProcessOutliers()
{
    //get pointer to the most recent entry in the pool
    uint16_t* this_pool = raw_channels[this_index];

    //get pointer to last index
    uint16_t* last_pool = raw_channels[last_index];


    //in all channels, see if the most recent value is an outlier of all the others (average)
    for(int i = 0; i < CHANNEL_COUNT; ++i)
    {
        uint32_t average_val = 0;
        for(int j = 0; j < AVERAGE_POOL_CNT; ++j)
            average_val += raw_channels[j][i];
        average_val /= AVERAGE_POOL_CNT;

        //is outlier, push out average
        if(abs(((int)this_pool[i] - (int)average_val)) > OUTLIER_THRESH)
            processed_channels[i] = (uint16_t)average_val;
        else //push out vanilla
            processed_channels[i] = this_pool[i];
    }




}

//read PPM values from the controller and run processOutliers
void ControllerHandler::GetControlSurface(uint16_t* channel_array, uint16_t array_len)
{

    //read PPM into packet struct
    for (byte channel = 1; channel <= array_len; ++channel) {

        //note: ppm.latestValidChannelValue is base-1!
        channel_array[channel - 1] = ppm->latestValidChannelValue(channel, channel_array[channel - 1]);
        Serial.printf("%d, \t", channel_array[channel - 1]);
    }
    Serial.println();


}

//take a channel array and normalize them according to their structs
void ControllerHandler::NormalizePPM(uint16_t raw_data, channel_analog_t* normalized_data)
{

    //truncate OOB
    if(raw_data > normalized_data->max_raw_val)
        raw_data = normalized_data->max_raw_val;
    else if(raw_data < normalized_data->min_raw_val)
        raw_data = normalized_data->min_raw_val;

    //normalize
    normalized_data->value_normalized = map(raw_data, normalized_data->min_raw_val, normalized_data->max_raw_val, NORMAL_MIN, NORMAL_MAX);

}

void ControllerHandler::ParseSwitchSums(uint16_t raw_data, channel_digital_t *profile)
{
    //normalize the analog portion of the signal
    NormalizePPM(raw_data, &profile->normalized_in);

    //for ease of typing
    auto normal_num = profile->normalized_in.value_normalized;

    //'0' state should be 256
    //NORMAL_MAX / 2

    //check for large state switch loations
    
    //9 states:

    //big switch
    //0
    //1
    //2

    //small switch
    //0
    //1
    //2



}





















