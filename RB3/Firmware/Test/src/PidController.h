#pragma once
#include <Arduino.h>

//better method using classes:
//https://stackoverflow.com/questions/5341639/is-my-implementation-of-fixed-point-arithmetic-correct
//note: this does not work for C, so I'm using the macros because of a class project that requires C...


//decimal ct.
#define Q32_SCALE 16

//operations
#define FIXED32_NEG(a) ((~a)+1)
#define FIXED32_ADD(a, b) (((a) + (b)) >> Q32_SCALE)
#define FIXED32_SUB(a, b) ((a) + FIXED32_NEG(b))
#define FIXED32_MUL(a, b) (((long long)(a) * (b)) >> Q32_SCALE)
#define FIXED32_DIV(a, b) (((long long)(a) << Q32_SCALE) / (b))

//data casting macros
#define FLOAT_TO_FIXED32(float_val) ((int32_t)(round(float_val * (1 << Q32_SCALE))))
#define FIXED32_TO_FLOAT(a) ((float)(a) / (1 << (Q32_SCALE)))
#define FIXED32_TO_INT(a) (a >> Q32_SCALE)
#define INT_TO_FIXED32(a) (a << Q32_SCALE)

typedef int32_t fixed32;


typedef struct PIDControllerF {

    //vars
    fixed32 p_const;
    fixed32 i_const;
    fixed32 d_const;

    //data retainers
    fixed32 last_pval;
    fixed32 ival;

    int32_t max_lim;
    int32_t min_lim;

    int32_t start_output_offset;

    bool has_limited_high;
    bool has_limited_low;

}PIDControllerF;


PIDControllerF* PIDControllerF_construct(
        int32_t start_output_offset,
        int32_t max_lim,
        int32_t min_lim,
        float p_const,
        float i_const,
        float d_const
);
void PIDControllerF_destruct(PIDControllerF* pc);

int32_t PIDControllerF_int_tick(PIDControllerF* pc, uint32_t target_rpm, uint32_t curr_rpm, uint32_t delta_micros);
int32_t PIDControllerF_tick(PIDControllerF* pc, fixed32 target_rpm, fixed32 curr_rpm, uint32_t delta_micros);
void PIDControllerF_set_output_offset(PIDControllerF* pc, int32_t offset);
void reset(PIDControllerF* pc);






