#include "divider_axi_1.h"

// // fixed_t safe_division(fixed_t dividend, fixed_t divisor) {
// //     #pragma HLS INLINE

// //     fixed_t safe_divisor = (divisor == fixed_t(0)) ? fixed_t(1) : divisor;
// //     fixed_t result = dividend / safe_divisor;

// //     if (divisor == fixed_t(0)) {
// //         result = (dividend >= fixed_t(0)) ? FixedPointMax : FixedPointMin;
// //     }
// //     return result;
// // }

fixed_t safe_division(fixed_t dividend, fixed_t divisor) {
    #pragma HLS INLINE
    if (divisor == fixed_t(0)) {
        if (dividend > fixed_t(0)) return FixedPointMax;
        if (dividend < fixed_t(0)) return FixedPointMin;
        return fixed_t(0);
    }
    return dividend / divisor;
}

// void divider_axi_1(hls::stream<axis_in_t>  &s_axis,
//                  hls::stream<axis_out_t> &m_axis) {
//     #pragma HLS INTERFACE axis       port=s_axis
//     #pragma HLS INTERFACE axis       port=m_axis
//     #pragma HLS INTERFACE ap_ctrl_none port=return
//     #pragma HLS PIPELINE II=1

//     if (!s_axis.empty() && !m_axis.full()) {
//         axis_in_t in_val = s_axis.read();

//         ap_int<8> dividend_raw = in_val.data.range(7,0);
//         ap_int<8> divisor_raw  = in_val.data.range(15,8);
//         fixed_t dividend = *reinterpret_cast<fixed_t*>(&dividend_raw);
//         fixed_t divisor  = *reinterpret_cast<fixed_t*>(&divisor_raw);

//         fixed_t result = safe_division(dividend, divisor);

//         axis_out_t out_val;
//         out_val.data = *reinterpret_cast<const ap_int<8>*>(&result);
//         out_val.last = in_val.last;
//         m_axis.write(out_val);
//     }
// }

void divider_axi_1(hls::stream<axis_in_t>  &s_axis,
                   hls::stream<axis_out_t> &m_axis) {
    #pragma HLS INTERFACE axis       port=s_axis
    #pragma HLS INTERFACE axis       port=m_axis
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS PIPELINE II=1

    // Static state: output register and valid flag
    static axis_out_t out_val;
    static bool       valid_reg = false;

    // FSM-style handshaking
    if (!valid_reg && !s_axis.empty()) {
        // Accept new input
        axis_in_t in_val = s_axis.read();

        ap_int<8> dividend_raw = in_val.data.range(7,0);
        ap_int<8> divisor_raw  = in_val.data.range(15,8);
        fixed_t dividend = *reinterpret_cast<fixed_t*>(&dividend_raw);
        fixed_t divisor  = *reinterpret_cast<fixed_t*>(&divisor_raw);

        fixed_t result = safe_division(dividend, divisor);

        out_val.data = *reinterpret_cast<const ap_int<8>*>(&result);
        out_val.last = in_val.last;
        valid_reg = true;  // output is now valid
    }

    if (valid_reg && !m_axis.full()) {
        // Output is ready to be consumed
        m_axis.write(out_val);
        valid_reg = false;  // free to accept new input
    }
}
