#pragma once
#include <ap_int.h>
#include <ap_fixed.h>

typedef ap_fixed<8, 5, AP_TRN, AP_SAT> fixed_t;

const fixed_t FixedPointMin = fixed_t(-16.0);
const fixed_t FixedPointMax = fixed_t(15.875);

void divider(
    fixed_t dividend_i,
    fixed_t divisor_i,
    bool    &in_ready_o,
    bool    in_valid_i,
    fixed_t &quotient_o,
    bool    out_ready_i,
    bool    &out_valid_o
);