#include <ap_int.h>
#include <ap_fixed.h>

// 1 sign + 4 integer + 3 fractional = 8 bits total
typedef ap_fixed<8, 5> fixed_t;
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
    bool valid_internal;
    // static bool valid_internal = false;


    in_ready_o = out_ready_i || !valid_internal;

    // IMPLEMENTATION 1 ------------------------------------------------------------
    // if (in_valid_i && in_ready_o) {
    //     // Vitis should be able to automatically figure out and handle this operation
    //     result = dividend_i / divisor_i;

    //     valid_internal = true;
    // static bool valid_internal = false;

    // }

    // if (ext_divisor == 0) {
    //     if      (ext_dividend < 0) result = FixedPointMin;
    //     else if (ext_dividend > 0) result = FixedPointMax;
    //     else                       result = 0;
    // } else if (result > FixedPointMax) {
    //     result = FixedPointMax;
    // } else if (result < FixedPointMin) {
    //     result = FixedPointMin;
    // }
    // -----------------------------------------------------------------------------

    // IMPLEMENTATION 2 ------------------------------------------------------------
    if (in_valid_i && in_ready_o) {
        ap_int<8> dividend_raw = *reinterpret_cast<ap_int<8>*>(&dividend_i);
        ap_int<8> divisor_raw  = *reinterpret_cast<ap_int<8>*>(&divisor_i);

        ap_int<16> ext_dividend = dividend_raw;
        ap_int<16> ext_divisor  = divisor_raw;

        fixed_t temp_result = 0;

        if (divisor_raw == 0) {
            // Divide-by-zero handling
            if (dividend_raw < 0)      temp_result = FixedPointMin;
            else if (dividend_raw > 0) temp_result = FixedPointMax;
            else                       temp_result = 0;
        } else {
            ap_int<16> numerator = ext_dividend << 3;  // Shift left to preserve fractional bits
            ap_int<16> quotient  = numerator / ext_divisor;

            // Extract bits [11:4] = 8-bit result with 3 fraction bits
            // ap_int<16> quantized_result = quotient.range(10, 3);

            // ap_int<16> quantized_result = quotient >> 3;
            // ap_int<8>  right_part       = quantized_result.range(10,3);

            ap_int<8> right_part = quotient;

            // Cast back to fixed-point type
            temp_result = *reinterpret_cast<fixed_t*>(&right_part);

            // Clamp to fixed-point range
            if (temp_result > FixedPointMax)      temp_result = FixedPointMax;
            else if (temp_result < FixedPointMin) temp_result = FixedPointMin;
        }

        result = temp_result;
        valid_internal = true;
    }
    // ----------------------------------------------------------------------

    quotient_o = result;
    out_valid_o = valid_internal;

    // If downstream is ready and output is valid, accept new data next cycle
    if (out_ready_i && out_valid_o) {
        valid_internal = false;
    }

}
