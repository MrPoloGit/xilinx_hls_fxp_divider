#include <iostream>
#include <iostream>
#include <cstdlib>
#include <bitset>
#include <ap_int.h>
#include <ap_fixed.h>
#include <vector>
#include "divider_axi_1.h"

struct test_case {
    fixed_t dividend;
    fixed_t divisor;
    fixed_t expected_result;
    std::string description;
};

fixed_t generate_random_fixed_t() {
    uint8_t raw = static_cast<uint8_t>(rand() % 256);
    ap_int<8> signed_val = static_cast<ap_int<8>>(raw);
    fixed_t fixed_val = *reinterpret_cast<fixed_t*>(&signed_val);
    return fixed_val;
}

int main() {
    hls::stream<axis_in_t> in_stream;
    hls::stream<axis_out_t> out_stream;

    std::cout << "GENERAL RANDOM TEST CASES" << std::endl;
    int num_tests = 100;
    for (int i = 0; i < num_tests; i++){
        fixed_t dividend = generate_random_fixed_t();
        fixed_t divisor  = generate_random_fixed_t();
        float expected_result;

        if (divisor == fixed_t(0)) {
            if      (dividend < fixed_t(0)) expected_result = (float)FixedPointMin;
            else if (dividend > fixed_t(0)) expected_result = (float)FixedPointMax;
            else                            expected_result = (float)0;
        } else {
            expected_result = dividend/divisor;
            if      (expected_result < (float)FixedPointMin) expected_result = (float)FixedPointMin;
            else if (expected_result > (float)FixedPointMax) expected_result = (float)FixedPointMax;
        }

        axis_in_t in_val;
        in_val.data.range(7,0)   = *reinterpret_cast<ap_int<8>*>(&dividend);
        in_val.data.range(15,8)  = *reinterpret_cast<ap_int<8>*>(&divisor);
        in_val.last = 1;
        in_stream.write(in_val);

        divider_axi_1(in_stream, out_stream);

        std::cout << "TEST " << i+1 << ": ";
        if (!out_stream.empty()) {
            axis_out_t out_val = out_stream.read();
            fixed_t result = *reinterpret_cast<fixed_t*>(&out_val.data);
            if (expected_result != (float)result) {
                std::cout << "  NOT EQUAL\n";
                std::cout << "  Dividend  " << dividend        << "\n";
                std::cout << "  Divisor   " << divisor         << "\n";
                std::cout << "  Result:   " << result          << "\n";
                std::cout << "  Expected: " << expected_result << "\n";
            } else {
                std::cout << "  PASS\n";
            }

        }
    }
    std::cout << std::endl;

    std::cout << "SPECIFIC TEST CASES" << std::endl;
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
        fixed_t dividend          = tests[i].dividend;
        fixed_t divisor           = tests[i].divisor;
        fixed_t expected_result = tests[i].expected_result;

        axis_in_t in_val;
        in_val.data.range(7,0)   = *reinterpret_cast<ap_int<8>*>(&dividend);
        in_val.data.range(15,8)  = *reinterpret_cast<ap_int<8>*>(&divisor);
        in_val.last = 1;
        in_stream.write(in_val);

        divider_axi_1(in_stream, out_stream);

        std::cout << "TEST " << i+1 << ": ";
        if (!out_stream.empty()) {
            axis_out_t out_val = out_stream.read();
            fixed_t result = *reinterpret_cast<fixed_t*>(&out_val.data);
            if (expected_result != result) {
                std::cout << "  NOT EQUAL\n";
                std::cout << "  Dividend  " << dividend          << "\n";
                std::cout << "  Divisor   " << divisor           << "\n";
                std::cout << "  Result:   " << result            << "\n";
                std::cout << "  Expected: " << expected_result << "\n";
            } else {
                std::cout << "  PASS\n";
            }
        }
        
    }
    std::cout << std::endl;

    std::cout << "FINISHED TEST BENCH" << std::endl;
    return 0;
}