#include <ap_int.h>
#include <ap_fixed.h>

// 1 sign + 4 integer + 3 fractional = 8 bits total
typedef ap_fixed<8, 5, AP_TRN, AP_WRAP> fixed_t;
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

    in_ready_o = out_ready_i || !valid_internal;

    if (in_valid_i && in_ready_o) {
        std::cout << "divider.cpp" << "\n";
        ap_int<8> dividend_raw = *reinterpret_cast<ap_int<8>*>(&dividend_i);
        ap_int<8> divisor_raw  = *reinterpret_cast<ap_int<8>*>(&divisor_i);

        ap_int<16> ext_dividend = dividend_raw;
        ap_int<16> ext_divisor  = divisor_raw;

        fixed_t temp_result = 0;

        std::cout << "  Pre Division:\n";
        std::cout << "      dividend_i:   " << dividend_i   << "\n";
        std::cout << "      divisor_i:    " << divisor_i    << "\n";
        std::cout << "      dividend_raw: " << dividend_raw << "\n";
        std::cout << "      divisor_raw:  " << divisor_raw  << "\n";
        std::cout << "      ext_dividend: " << ext_dividend << "\n";
        std::cout << "      ext_divisor:  " << ext_divisor  << "\n";

        if (divisor_i == fixed_t(0)) {
            // Divide-by-zero handling
            if      (dividend_i < fixed_t(0)) temp_result = FixedPointMin;
            else if (dividend_i > fixed_t(0)) temp_result = FixedPointMax;
            else                              temp_result = fixed_t(0);
            
            std::cout << "  Division by zero detected, result: " << temp_result << "\n";
        } else {
            ap_int<16> numerator = ext_dividend << 3;  // Shift left to preserve fractional bits
            ap_int<16> quotient  = numerator / ext_divisor;
            ap_int<8>  scaled_q  = quotient;
            fixed_t test_q = fixed_t(quotient);

            // Clamp to fixed-point range
            // Should be doing here using quotient, like in rowwise_div.sv, however
            // when attempted it was failing, but passing others so I'm doing something wrong
            if      (test_q > FixedPointMax) temp_result = FixedPointMax;
            else if (test_q < FixedPointMin) temp_result = FixedPointMin;
            else    temp_result = *reinterpret_cast<fixed_t*>(&scaled_q);

            std::cout << "  After Division:\n";
            std::cout << "      numerator:   " << numerator   << "\n";
            std::cout << "      quotient:    " << quotient    << "\n";
            std::cout << "      scaled_q:    " << scaled_q    << "\n";
            std::cout << "      temp_result: " << temp_result << "\n";

            // if      (temp_result > FixedPointMax) temp_result = FixedPointMax;
            // else if (temp_result < FixedPointMin) temp_result = FixedPointMin;
        }

        result = temp_result;
        valid_internal = true;
    }

    quotient_o = result;
    out_valid_o = valid_internal;

    // If downstream is ready and output is valid, accept new data next cycle
    if (out_ready_i && out_valid_o) {
        valid_internal = false;
    }

}