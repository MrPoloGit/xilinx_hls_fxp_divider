#include <iostream>
#include "divider.h"

// fixed_t safe_division(fixed_t dividend, fixed_t divisor) {
//     #pragma HLS INLINE

//     fixed_t safe_divisor = (divisor == fixed_t(0)) ? fixed_t(1) : divisor;
//     fixed_t result = dividend / safe_divisor;

//     if (divisor == fixed_t(0)) {
//         result = (dividend >= fixed_t(0)) ? FixedPointMax : FixedPointMin;
//     }
//     return result;
// }

fixed_t safe_division(fixed_t dividend, fixed_t divisor) {
    #pragma HLS INLINE
    if (divisor == fixed_t(0)) {
        if (dividend > fixed_t(0)) return FixedPointMax;
        if (dividend < fixed_t(0)) return FixedPointMin;
        return fixed_t(0);
    }
    return dividend / divisor;
}

void divider(
    fixed_t dividend_i,
    fixed_t divisor_i,
    bool    &in_ready_o,
    bool    in_valid_i,
    fixed_t &quotient_o,
    bool    out_ready_i,
    bool    &out_valid_o
) {
    #pragma HLS INTERFACE ap_none port=dividend_i
    #pragma HLS INTERFACE ap_none port=divisor_i
    #pragma HLS INTERFACE ap_none port=in_ready_o
    #pragma HLS INTERFACE ap_none port=in_valid_i
    #pragma HLS INTERFACE ap_none port=quotient_o
    #pragma HLS INTERFACE ap_none port=out_ready_i
    #pragma HLS INTERFACE ap_none port=out_valid_o
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    fixed_t result = 0;
    bool valid_internal;

    in_ready_o = out_ready_i || !valid_internal;

    if (in_valid_i && in_ready_o) {
        result = safe_division(dividend_i, divisor_i);
        valid_internal = true;
    }

    quotient_o = result;
    out_valid_o = valid_internal;

    // If downstream is ready and output is valid, accept new data next cycle
    if (out_ready_i && out_valid_o) {
        valid_internal = false;
    }

}