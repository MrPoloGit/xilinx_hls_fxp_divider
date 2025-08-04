#include <iostream>
#include <cstdlib>
#include <bitset>
#include <ap_int.h>
#include <ap_fixed.h>
#include <vector>

// 1 sign + 4 integer + 3 fractional = 8 bits total
typedef ap_fixed<8, 5, AP_TRN, AP_SAT> fixed_t;
static ap_int<8> raw_min = 0b10000000; 
static ap_int<8> raw_max = 0b01111111;
static fixed_t FixedPointMin = *reinterpret_cast<fixed_t*>(&raw_min); // -16.0
static fixed_t FixedPointMax = *reinterpret_cast<fixed_t*>(&raw_max); // 15.875

struct test_case {
    fixed_t dividend;
    fixed_t divisor;
    fixed_t expected_result;
    std::string description;
};

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

    return fixed_val;
}

int main() {
    fixed_t dividend_i, divisor_i, quotient_o;
    bool in_ready_o, in_valid_i, out_ready_i, out_valid_o;
    float expected_quotient;

    // Standard Random Test
    const int num_tests = 60;
    for (int i = 0; i < num_tests; i++) {
        std::cout << "Test: " << i << "\n";
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
        if      (expected_quotient > (float)FixedPointMax) expected_quotient = (float)FixedPointMax;
        else if (expected_quotient < (float)FixedPointMin) expected_quotient = (float)FixedPointMin;

        divider(dividend_i, divisor_i, in_ready_o, in_valid_i, quotient_o, out_ready_i, out_valid_o);

        if (out_valid_o) {
            if (expected_quotient != (float)quotient_o) {
                std::cout << "  NOT EQUAL   " << "\n";
                std::cout << "  Result:   " << quotient_o        << "\n";
                std::cout << "  Expected: " << expected_quotient << "\n"; 
            }
            
        }
        std::cout << std::endl;
    }

    // Specific Unique Test Cases
    std::vector<test_case> tests = {
        {7.75,    0.125,  15.875, "Same sign overflow check"},
        {10.125,  -0.125, -16,    "Different sign overflow check"},
        {0.0,     2.125,  0.0,    "Dividend is 0, Divisor is Positive"},
        {3.25,    0,      15.875, "Positive Dividend, Divisor is 0"},
        {0,       -0.5,   0,      "Dividend is 0, Divisor is Negative"},
        {-10.0,   0,      -16,    "Negative dividend, Divisor is 0"},
        {-14.875, -0.875, 15.875, "Divisor is negative, Divisor is negative, result is FixedPointMax"}
    };

    for (size_t i = 0; i < tests.size(); i++) {
        std::cout << "Test: " << i << "\n";
        // Pre set up
        dividend_i = 0;
        divisor_i = 1;
        in_valid_i = false;
        divider(dividend_i, divisor_i, in_ready_o, in_valid_i, quotient_o, out_ready_i, out_valid_o);
        
        // Input valid information
        dividend_i = tests[i].dividend;
        divisor_i  = tests[i].divisor;
        in_valid_i = true;
        if (divisor_i == fixed_t(0)) {
            expected_quotient = 0;
        } else {
            expected_quotient = dividend_i/divisor_i;
            if      (expected_quotient > (float)FixedPointMax) expected_quotient = (float)FixedPointMax;
            else if (expected_quotient < (float)FixedPointMin) expected_quotient = (float)FixedPointMin;
        }
        divider(dividend_i, divisor_i, in_ready_o, in_valid_i, quotient_o, out_ready_i, out_valid_o);

        if (out_valid_o) {
            if (tests[i].expected_result != quotient_o) {
                std::cout << "  NOT EQUAL   " << "\n";
                std::cout << "  Dividend: " << dividend_i            << "\n";
                std::cout << "  Divisor:  " << divisor_i             << "\n";
                std::cout << "  Result:   " << quotient_o            << "\n";
                std::cout << "  Expected: " << tests[i].expected_result << "\n"; 
            }
            
        }
        std::cout << std::endl;
    }

    std::cout << "FINISHED TEST BENCH" << std::endl;
    return 0;
}
