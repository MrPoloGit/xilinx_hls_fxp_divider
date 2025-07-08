#include <iostream>
#include "ap_int.h"

typedef ap_int<8> fixed8_t;

void divider(
    fixed8_t lhs,
    fixed8_t rhs,
    bool     valid_in,
    bool     ready_in,
    fixed8_t &quotient,
    bool     &valid_out,
    bool     &ready_out
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
    bool valid_out, ready_out;

    divider(lhs, rhs, true, true, quotient, valid_out, ready_out);

    std::cout << "Result (float): " << to_float(quotient) << "\n";
    std::cout << "Valid: " << valid_out << ", Ready: " << ready_out << "\n";

    return 0;
}
