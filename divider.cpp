#include <iostream>
#include <ap_int.h>
#include <ap_fixed.h>

// 1 sign + 4 integer + 3 fractional = 8 bits total
typedef ap_fixed<8, 5, AP_TRN, AP_SAT> fixed_t;
// Not sure why limits isn't working for this
static fixed_t FixedPointMin = fixed_t(-16.0);
static fixed_t FixedPointMax = fixed_t(15.875);

fixed_t safe_division(fixed_t dividend, fixed_t divisor) {
    #pragma HLS INLINE

    fixed_t safe_divisor = (divisor == 0) ? fixed_t(1) : divisor;
    fixed_t result = dividend / safe_divisor;

    if (divisor == 0) {
        result = (dividend >= 0) ? FixedPointMax : FixedPointMin;
    }
    return result;
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