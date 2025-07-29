#include <iostream>
#include <cstdlib>
#include <bitset>
#include "ap_int.h"

// 1 sign + 4 integer + 3 fractional = 8 bits total
typedef ap_fixed<8, 5> fixed8_t;

void divider(
    fixed8_t lhs,
    fixed8_t rhs,
    bool     &in_ready_o,
    bool     in_valid_i,
    fixed8_t &quotient,
    bool     out_ready_i,
    bool     &out_valid_o
);

float to_float(fixed8_t val) {
    return (float)val;
}

fixed8_t from_float(float val) {
    return (fixed8_t)(val * 8.0f);
}

fixed8_t generate_random_fixed8_t() {
    uint8_t raw = 0b01111111; // 127
    // uint8_t raw = static_cast<uint8_t>(rand() % 256);
    ap_int<8> signed_val = static_cast<ap_int<8>>(raw);
    std::bitset<8> x(signed_val);
    fixed8_t fixed_val = *reinterpret_cast<fixed8_t*>(&signed_val);
    std::bitset<8> y(fixed_val); // it gives the quantized fixed-point value converted to an integer

    std::cout << "generate_random_fixed8_t: raw: " << raw << "\n";
    std::cout << "generate_random_fixed8_t: signed_val: " << signed_val << "\n";
    std::cout << "generate_random_fixed8_t: signed_val bitset: " << x << "\n";
    std::cout << "generate_random_fixed8_t: fixed_val: " << fixed_val << "\n";
    std::cout << "generate_random_fixed8_t: fixed_val bitset: " << y << "\n";

    return fixed_val;
}

// Temporary functions to raw test
fixed8_t generate_random_fixed8_t_1() {
    uint8_t raw = 0b00000010; // 0.5
    ap_int<8> signed_val = static_cast<ap_int<8>>(raw);
    std::bitset<8> x(signed_val);
    fixed8_t fixed_val = *reinterpret_cast<fixed8_t*>(&signed_val);
    std::bitset<8> y(fixed_val); // it gives the quantized fixed-point value converted to an integer

    return fixed_val;
}

fixed8_t generate_random_fixed8_t_2() {
    uint8_t raw = 0b00001000; // 1
    ap_int<8> signed_val = static_cast<ap_int<8>>(raw);
    std::bitset<8> x(signed_val);
    fixed8_t fixed_val = *reinterpret_cast<fixed8_t*>(&signed_val);
    std::bitset<8> y(fixed_val); // it gives the quantized fixed-point value converted to an integer

    return fixed_val;
}

// ap_int<8> raw_bits = *reinterpret_cast<ap_int<8>*>(&fixed_val);

int main() {
    fixed8_t lhs, rhs, quotient;
    float expected_quotient;

    bool in_valid_i = true;
    bool in_ready_o;
    bool out_ready_i = true;
    bool out_valid_o;

    // Simulate multiple clock cycles
    const int num_tests = 10;
    for (int cycle = 0; cycle < num_tests; ++cycle) {
        lhs = generate_random_fixed8_t_1();
        rhs = generate_random_fixed8_t_2();
        expected_quotient = to_float(lhs)/to_float(rhs);
        divider(lhs, rhs, in_ready_o, in_valid_i, quotient, out_ready_i, out_valid_o);

        std::cout << "Cycle " << cycle << ":\n";
        std::cout << "  lhs = " << to_float(lhs) << "\n";
        std::cout << "  rhs = " << to_float(rhs) << "\n";
        std::cout << "  Expected Result = " << expected_quotient << "\n";
        if (out_valid_o) {
            std::cout << "  Divider Result = " << to_float(quotient);
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
