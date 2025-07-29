#include <iostream>
#include <cstdlib>
#include <bitset>
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
    bool     &in_ready_o,
    bool     in_valid_i,
    fixed8_t &quotient,
    bool     out_ready_i,
    bool     &out_valid_o
);

fixed8_t generate_random_fixed8_t() {
    // uint8_t raw = 0b01111111; // 127
    uint8_t raw = static_cast<uint8_t>(rand() % 256);
    ap_int<8> signed_val = static_cast<ap_int<8>>(raw);
    std::bitset<8> x(signed_val);
    fixed8_t fixed_val = *reinterpret_cast<fixed8_t*>(&signed_val);
    // ap_int<8> raw_bits = *reinterpret_cast<ap_int<8>*>(&fixed_val);
    std::bitset<8> y(fixed_val); // it gives the quantized fixed-point value converted to an integer

    std::cout << "generate_random_fixed8_t: raw: " << raw << "\n";
    std::cout << "generate_random_fixed8_t: signed_val: " << signed_val << "\n";
    std::cout << "generate_random_fixed8_t: signed_val bitset: " << x << "\n";
    std::cout << "generate_random_fixed8_t: fixed_val: " << fixed_val << "\n";
    std::cout << "generate_random_fixed8_t: fixed_val bitset: " << y << "\n";

    return fixed_val;
}

int main() {
    fixed8_t dividend_i, divisor_i, quotient;
    float expected_quotient;

    bool in_valid_i = true;
    bool in_ready_o;
    bool out_ready_i = true;
    bool out_valid_o;

    // Simulate multiple clock cycles
    const int num_tests = 10;
    for (int cycle = 0; cycle < num_tests; ++cycle) {
        dividend_i = generate_random_fixed8_t();
        divisor_i = generate_random_fixed8_t();
        expected_quotient = dividend_i/divisor_i;
        divider(dividend_i, divisor_i, in_ready_o, in_valid_i, quotient, out_ready_i, out_valid_o);

        std::cout << "Cycle " << cycle << ":\n";
        std::cout << "  dividend_i = " << dividend_i << "\n";
        std::cout << "  divisor_i = " << divisor_i << "\n";
        if (out_valid_o) {
            std::cout << "  Expected Result = " << expected_quotient << "\n";
            std::cout << "  Divider Result = " << quotient;
        }
        std::cout << std::endl;

        // Temporary measure to prevent a current crash that is occuring
        if (in_ready_o && in_valid_i) {
            in_valid_i = false;
        }

        if (out_valid_o && out_ready_i) {
            break;
        }
    }

    return 0;
}
