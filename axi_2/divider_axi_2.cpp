#include "divider_axi_2.h"

// Safe division with saturation
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

// Stage 1: Read from AXI stream and unpack operands
static void read_inputs(hls::stream<axis_in_t> &s_axis,
                        hls::stream<fixed_t>   &dividend_fifo,
                        hls::stream<fixed_t>   &divisor_fifo) {
    if (!s_axis.empty()) {
        axis_in_t in_val = s_axis.read();
        ap_int<8> dividend_raw = in_val.data.range(7,0);
        ap_int<8> divisor_raw  = in_val.data.range(15,8);
        fixed_t dividend = *reinterpret_cast<fixed_t*>(&dividend_raw);
        fixed_t divisor  = *reinterpret_cast<fixed_t*>(&divisor_raw);
        dividend_fifo.write(dividend);
        divisor_fifo.write(divisor);
    }
}

// Stage 2: Compute division
static void compute_division(hls::stream<fixed_t> &dividend_fifo,
                              hls::stream<fixed_t> &divisor_fifo,
                              hls::stream<fixed_t> &result_fifo) {
    if (!dividend_fifo.empty() && !divisor_fifo.empty()) {
        fixed_t dividend = dividend_fifo.read();
        fixed_t divisor  = divisor_fifo.read();
        fixed_t result   = safe_division(dividend, divisor);
        result_fifo.write(result);
    }
}

// Stage 3: Write result to AXI output
static void write_outputs(hls::stream<fixed_t> &result_fifo,
                          hls::stream<axis_out_t> &m_axis) {
    if (!result_fifo.empty()) {
        fixed_t result = result_fifo.read();
        axis_out_t out_val;
        out_val.data = *reinterpret_cast<ap_int<8>*>(&result);
        m_axis.write(out_val);
    }
}

// Top-level AXI divider
void divider_axi_2(hls::stream<axis_in_t>  &s_axis,
                   hls::stream<axis_out_t> &m_axis) {
    #pragma HLS INTERFACE axis       port=s_axis
    #pragma HLS INTERFACE axis       port=m_axis
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS DATAFLOW

    // Internal FIFOs
    hls::stream<fixed_t> dividend_fifo("dividend_fifo"), divisor_fifo("divisor_fifo"), result_fifo("result_fifo");
    #pragma HLS STREAM variable=dividend_fifo depth=4
    #pragma HLS STREAM variable=divisor_fifo depth=4
    #pragma HLS STREAM variable=result_fifo  depth=4

    read_inputs(s_axis, dividend_fifo, divisor_fifo);
    compute_division(dividend_fifo, divisor_fifo, result_fifo);
    write_outputs(result_fifo, m_axis);
}
