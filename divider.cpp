#include <ap_int.h>
#include <hls_stream.h>

// 1 sign + 4 integer + 3 fractional = 8 bits total
typedef ap_int<8> fixed8_t;

void divider(
    fixed8_t lhs,
    fixed8_t rhs,
    bool     valid_in,
    bool     ready_in,
    fixed8_t &quotient,
    bool     &valid_out,
    bool     &ready_out
) {
    #pragma HLS INTERFACE ap_none port=lhs
    #pragma HLS INTERFACE ap_none port=rhs
    #pragma HLS INTERFACE ap_none port=valid_in
    #pragma HLS INTERFACE ap_none port=ready_in
    #pragma HLS INTERFACE ap_none port=quotient
    #pragma HLS INTERFACE ap_none port=valid_out
    #pragma HLS INTERFACE ap_none port=ready_out
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    static bool valid_internal = false;
    static fixed8_t result = 0;

    if (valid_in && ready_in) {
        // Extend to prevent overflow during division
        ap_int<16> lhs_ext = lhs;
        ap_int<16> rhs_ext = rhs;

        // Align fixed-point for division
        ap_int<16> dividend = lhs_ext << 3; // scale lhs
        ap_int<16> quotient_ext = dividend / rhs_ext;

        result = quotient_ext.range(7, 0); // truncate
        valid_internal = true;
    }

    quotient = result;
    valid_out = valid_internal;
    ready_out = 1; // always ready to accept output

    // Clear valid if downstream is ready
    if (ready_in && valid_internal) {
        valid_internal = false;
    }
}
