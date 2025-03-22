#pragma once

#include <Arduino.h>


class Apple {

    Apple();


    ~Apple() {

    }

    private:

    int32_t motor_output3 = 0;
    int32_t motor_outputf = 0;

    bool remote_disable = false;
    //current enable state
    bool is_enabled = false;

void MapControllerData();


};





