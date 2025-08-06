#pragma once
#include <ap_int.h>
#include <ap_fixed.h>
#include <hls_stream.h>

typedef ap_fixed<8, 5, AP_TRN, AP_SAT> fixed_t;

const fixed_t FixedPointMin = fixed_t(-16.0);
const fixed_t FixedPointMax = fixed_t(15.875);

struct axis_in_t {
    ap_uint<16> data;  // [15:8] divisor, [7:0] dividend
    ap_uint<1>  last;
};

struct axis_out_t {
    ap_int<8>  data;  
    ap_uint<1> last;
};

void divider_axi_2(hls::stream<axis_in_t>  &s_axis,
                   hls::stream<axis_out_t> &m_axis);
