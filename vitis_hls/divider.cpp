#include <ap_int.h>

// 1 sign + 4 integer + 3 fractional = 8 bits total
typedef ap_fixed<8, 5> fixed8_t;

void divider(
    fixed8_t lhs,
    fixed8_t rhs,
    bool     &in_ready_o,     // tells upstream "I'm ready to accept input"
    bool     in_valid_i,      // tells this module "upstream has valid input"
    fixed8_t &quotient,
    bool     out_ready_i,     // tells this module "downstream is ready to accept output"
    bool     &out_valid_o     // tells downstream "I have valid output"
) {
    #pragma HLS INTERFACE ap_none port=lhs
    #pragma HLS INTERFACE ap_none port=rhs
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
        // Perform fixed-point division (scaled by 2^3 = 8)
        // ap_int<16> lhs_ext = lhs;
        // ap_int<16> rhs_ext = rhs;
        // ap_int<16> dividend = lhs_ext << 3; // shift left to align fixed-point, I believe this may be a mistake
        // ap_int<16> divisor = rhs;
        // ap_int<16> quotient_ext = dividend / divisor;
        // result = quotient_ext.range(7, 0);  // Truncate back to 8-bit

        // Vitis should be able to automatically figure out and handle this operation
        result = lhs / rhs;
        valid_internal = true;

        // #ifndef __SYNTHESIS__
        //     std::cout << "divider: lhs: " << lhs << ", rhs: " << rhs << "\n";
        //     std::cout << "divider: lhs_ext: " << lhs_ext << ", rhs_ext: " << rhs_ext << "\n";
        //     std::cout << "divider: dividend: " << dividend << "\n";
        //     std::cout << "divider: quotient_ext: " << quotient_ext << "\n";
        //     std::cout << "divider: result (8-bit): " << result << "\n";
        // #endif
    }

    quotient = result;
    out_valid_o = valid_internal;

    // If downstream is ready and output is valid, accept new data next cycle
    if (out_ready_i && valid_internal) {
        valid_internal = false;
    }
}
