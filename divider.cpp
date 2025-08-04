#include <iostream>
#include <ap_int.h>
#include <ap_fixed.h>

// 1 sign + 4 integer + 3 fractional = 8 bits total
typedef ap_fixed<8, 5, AP_TRN, AP_SAT> fixed_t;
// Not sure why limits isn't working for this
static ap_int<8> raw_min = 0b10000000; 
static ap_int<8> raw_max = 0b01111111;
static fixed_t FixedPointMin = *reinterpret_cast<fixed_t*>(&raw_min); // -16.0
static fixed_t FixedPointMax = *reinterpret_cast<fixed_t*>(&raw_max); // 15.875

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

    static fixed_t result = 0;
    static bool valid_internal = false;

    in_ready_o = !valid_internal;

    if (in_valid_i && in_ready_o) {
        if (divisor_i == fixed_t(0)) {
            // Divide-by-zero handling
            if      (dividend_i < fixed_t(0)) result = FixedPointMin;
            else if (dividend_i > fixed_t(0)) result = FixedPointMax;
            else                              result = fixed_t(0);    // Redundant TBH
        } else {
            // The ap_fixed library handles the scaling automatically
            result = dividend_i / divisor_i;
        }

        valid_internal = true;
    }

    quotient_o = result;
    out_valid_o = valid_internal;

    // If downstream is ready and output is valid, accept new data next cycle
    if (out_ready_i && out_valid_o) {
        valid_internal = false;
    }

}
