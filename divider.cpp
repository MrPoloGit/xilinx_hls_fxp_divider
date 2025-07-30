#include <ap_int.h>
#include <ap_fixed.h>

// 1 sign + 4 integer + 3 fractional = 8 bits total
typedef ap_fixed<8, 5> fixed8_t;
static ap_int<8> raw_min = 0b10000000; 
static ap_int<8> raw_max = 0b01111111;
static fixed8_t FixedPointMin = *reinterpret_cast<fixed8_t*>(&raw_min); // -16.0
static fixed8_t FixedPointMax = *reinterpret_cast<fixed8_t*>(&raw_max); // 15.875

void divider(
    fixed8_t dividend_i,
    fixed8_t divisor_i,
    bool     &in_ready_o,     // tells upstream "I'm ready to accept input"
    bool     in_valid_i,      // tells this module "upstream has valid input"
    fixed8_t &quotient_o,
    bool     out_ready_i,     // tells this module "downstream is ready to accept output"
    bool     &out_valid_o     // tells downstream "I have valid output"
) {
    #pragma HLS INTERFACE ap_none port=dividend_i
    #pragma HLS INTERFACE ap_none port=divisor_i
    #pragma HLS INTERFACE ap_none port=in_ready_o
    #pragma HLS INTERFACE ap_none port=in_valid_i
    #pragma HLS INTERFACE ap_none port=quotient
    #pragma HLS INTERFACE ap_none port=out_ready_i
    #pragma HLS INTERFACE ap_none port=out_valid_o
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static bool valid_internal = false;
    static fixed8_t result = 0;

    // Ready to accept input if internal register is not full
    in_ready_o = !valid_internal;

    if (in_valid_i && in_ready_o) {
        // Vitis should be able to automatically figure out and handle this operation
        result = dividend_i / divisor_i;
        valid_internal = true;
    }

    quotient = result;
    out_valid_o = valid_internal;

    // If downstream is ready and output is valid, accept new data next cycle
    if (out_ready_i && valid_internal) {
        valid_internal = false;
    }
}
