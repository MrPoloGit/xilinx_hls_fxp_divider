#include <iostream>
#include <cstdlib>
#include <bitset>
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
);

fixed_t generate_random_fixed_t() {
    uint8_t raw = static_cast<uint8_t>(rand() % 256);
    ap_int<8> signed_val = static_cast<ap_int<8>>(raw);
    std::bitset<8> x(signed_val);
    fixed_t fixed_val = *reinterpret_cast<fixed_t*>(&signed_val);
    std::bitset<8> y(fixed_val); // it gives the quantized fixed-point value converted to an integer

    std::cout << "generate_random_fixed_t: raw: "               << raw        << "\n";
    std::cout << "generate_random_fixed_t: signed_val: "        << signed_val << "\n";
    std::cout << "generate_random_fixed_t: signed_val bitset: " << x          << "\n";
    std::cout << "generate_random_fixed_t: fixed_val: "         << fixed_val  << "\n";
    std::cout << "generate_random_fixed_t: fixed_val bitset: "  << y          << "\n";

    return fixed_val;
}

int main() {
    fixed_t dividend_i, divisor_i, quotient_o;
    bool in_ready_o, in_valid_i, out_ready_i, out_valid_o;
    float expected_quotient;

    const int num_tests = 60;
    for (int test_num = 0; test_num < num_tests; ++test_num) {
        std::cout << "Test: " << test_num << "\n";
        // Pre set up
        dividend_i = 0;
        divisor_i = 1;
        in_valid_i = false;
        divider(dividend_i, divisor_i, in_ready_o, in_valid_i, quotient_o, out_ready_i, out_valid_o);
        
        // Input valid information
        dividend_i = generate_random_fixed_t();
        divisor_i = generate_random_fixed_t();
        in_valid_i = true;
        expected_quotient = dividend_i/divisor_i;
        divider(dividend_i, divisor_i, in_ready_o, in_valid_i, quotient_o, out_ready_i, out_valid_o);

        if (out_valid_o) {
            if (expected_quotient != quotient_o) {
                std::cout << "  NOT EQUAL   " << "\n";
            }
            std::cout << "  Dividend: " << dividend_i        << "\n";
            std::cout << "  Divisor:  " << divisor_i         << "\n";
            std::cout << "  Result:   " << quotient_o        << "\n";
            std::cout << "  Expected: " << expected_quotient << "\n"; 
        }
        std::cout << std::endl;
    }

    return 0;
}
