#include <iostream>
#include "ap_int.h"

typedef ap_int<8> fixed8_t;

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
    return (float)val / 8.0f;
}

fixed8_t from_float(float val) {
    return (fixed8_t)(val * 8.0f);
}

int main() {
    fixed8_t lhs = from_float(6.25f);
    fixed8_t rhs = from_float(1.25f);
    fixed8_t quotient;

    bool in_valid_i = true;
    bool in_ready_o;
    bool out_ready_i = true;
    bool out_valid_o;

    // Simulate multiple clock cycles
    const int max_cycles = 10;
    for (int cycle = 0; cycle < max_cycles; ++cycle) {
        divider(lhs, rhs, in_ready_o, in_valid_i, quotient, out_ready_i, out_valid_o);

        std::cout << "Cycle " << cycle << ": ";
        std::cout << "in_ready_o=" << in_ready_o << ", ";
        std::cout << "out_valid_o=" << out_valid_o << ", ";
        if (out_valid_o) {
            std::cout << "Result = " << to_float(quotient);
        }
        std::cout << std::endl;

        // After one input is accepted, stop asserting valid
        if (in_ready_o && in_valid_i) {
            in_valid_i = false;
        }

        // After one valid output is seen, stop test
        if (out_valid_o && out_ready_i) {
            break;
        }
    }

    return 0;
}
