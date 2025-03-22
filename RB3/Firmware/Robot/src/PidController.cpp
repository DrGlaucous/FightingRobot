#include <Arduino.h>
#include <math.h>
#include "PidController.h"

#include "configuration.h"



//motor is shorted, braking enabled
//HIGH-HIGH
//LOW-LOW

//direction 1
//HIGH-LOW

//direction 2
//LOW-HIGH


//PID input is motor speed in encoder units (RPM * 200)
//PID output is motor "on" percent (from -255 to 255)



//writing this in C so I can use it with my graded project


//create a pointer to a set of PID values
PIDControllerF* PIDControllerF_construct(
        int32_t start_output_offset,
        int32_t max_lim,
        int32_t min_lim,
        float p_const,
        float i_const,
        float d_const
) {
    PIDControllerF* pc = (PIDControllerF*)malloc(sizeof(PIDControllerF));
    pc->p_const = FLOAT_TO_FIXED32(p_const);
    pc->i_const = FLOAT_TO_FIXED32(i_const);
    pc->d_const = FLOAT_TO_FIXED32(d_const);
    pc->last_pval = FLOAT_TO_FIXED32(0.0);
    pc->ival = FLOAT_TO_FIXED32(0.0);
    pc->max_lim = max_lim;
    pc->min_lim = min_lim;
    pc->start_output_offset = 0;
    pc->has_limited_high = false;
    pc->has_limited_low = false;

    return pc;
}

//destroy the PID variable
void PIDControllerF_destruct(PIDControllerF* pc) {
    if(pc) {
        free(pc);
        pc = NULL;
    }
}

//same as `PIDControllerF_tick`, but takes an int as an input for target and current
int32_t PIDControllerF_int_tick(PIDControllerF* pc, uint32_t target_rpm, uint32_t curr_rpm, uint32_t delta_micros) {
    return PIDControllerF_tick(pc, INT_TO_FIXED32(target_rpm), INT_TO_FIXED32(curr_rpm), delta_micros);
}


//runs the pid loop for one cycle and returns the current calculated PID value (takes fixed point)
int32_t PIDControllerF_tick(PIDControllerF* pc, fixed32 target_rpm, fixed32 curr_rpm, uint32_t delta_micros) {

    //small RPM = big throttle value, direct acting

    //delta_micros = 200; //for testing

    //determine PID values

    //proportional change
    fixed32 pval = FIXED32_SUB(target_rpm, curr_rpm);

    fixed32 delta_micros_f32 = INT_TO_FIXED32(delta_micros);

    //if our value is increasing/decreasing and we limited the output, don't continue integration
    //or if our current value is 0 (rely only on P control for startup)
    if(
        !(
        (pval > 0 && pc->has_limited_high) ||
        (pval < 0 && pc->has_limited_low)
        )
        && curr_rpm != 0
    ) {
        //ival += pval * i_const * (float)delta_micros;

        fixed32 mults = FIXED32_MUL(FIXED32_MUL(pval, pc->i_const), delta_micros_f32);
        pc->ival = FIXED32_ADD(pc->ival, mults);
    }

    fixed32 dval = FIXED32_SUB(pval, pc->last_pval);
    pc->last_pval = pval;




    // //sum and multiply by constants
    // int32_t pid_out = (int32_t)(
    //     pval * p_const * (float)delta_micros
    //     + ival //integral constant is applied above
    //     + dval * d_const * (float)delta_micros
    // ) + start_output_offset;

    //very messy: limited macros
    int32_t pid_out = FIXED32_TO_INT(
        FIXED32_ADD(
            FIXED32_ADD(
                FIXED32_MUL(FIXED32_MUL(pval, pc->p_const), delta_micros_f32), //P
                pc->ival //I (delta_micros already applied above)
            ),
            FIXED32_MUL(FIXED32_MUL(dval, pc->d_const), delta_micros_f32) //D
        )
    ) + pc->start_output_offset;


    //limit output
    if(pid_out > pc->max_lim) {
        pid_out = pc->max_lim;
        pc->has_limited_high = true;
        pc->has_limited_low = false;
    } else if (pid_out < pc->min_lim) {
        pid_out = pc->min_lim;
        pc->has_limited_high = false;
        pc->has_limited_low = true;
    } else {
        pc->has_limited_high = false;
        pc->has_limited_low = false;
    }

    return(pid_out);

}

//set the output offset, added onto the PID loop's output each tick
void PIDControllerF_set_output_offset(PIDControllerF* pc, int32_t offset) {
    fixed32 fp_offset = INT_TO_FIXED32(offset);

    if(fp_offset < pc->min_lim) {
        pc->start_output_offset = FIXED32_TO_INT(pc->min_lim);
    } else if(fp_offset > pc->max_lim) {
        pc->start_output_offset = FIXED32_TO_INT(pc->max_lim);
    } else {
        pc->start_output_offset = offset;
    }
}

//called whenever we take manual control of the loop, resets persistent I and D values
void reset(PIDControllerF* pc) {
    pc->last_pval = INT_TO_FIXED32(0);
    pc->ival = INT_TO_FIXED32(0);
    pc->has_limited_high = false;
    pc->has_limited_low = false;
}





