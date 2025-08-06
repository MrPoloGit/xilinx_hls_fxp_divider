#include <iostream>
#include <cstdlib>
#include <bitset>
#include <vector>
#include "divider_axi.h"

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

void run_and_drain(hls::stream<axis_in_t> &in_stream,
                   hls::stream<axis_out_t> &out_stream,
                   int flush_count,
                   std::vector<fixed_t> &results) {
    // Flush the pipeline by calling the design enough times
    for (int i = 0; i < flush_count; i++) {
        divider_axi(in_stream, out_stream);
        if (!out_stream.empty()) {
            axis_out_t out_val = out_stream.read();
            results.push_back(*reinterpret_cast<fixed_t*>(&out_val.data));
        }
    }
}

int main() {
    hls::stream<axis_in_t> in_stream;
    hls::stream<axis_out_t> out_stream;

    std::cout << "GENERAL RANDOM TEST CASES" << std::endl;
    const int num_tests = 100;
    std::vector<fixed_t> dividends, divisors, rand_results, rand_outputs;

    for (int i = 0; i < num_tests; i++) {
        fixed_t dividend = generate_random_fixed_t();
        fixed_t divisor  = generate_random_fixed_t();
        fixed_t expected_result;

        // Expected computation
        if (divisor == fixed_t(0)) {
            if      (dividend < fixed_t(0)) expected_result = FixedPointMin;
            else if (dividend > fixed_t(0)) expected_result = FixedPointMax;
            else                            expected_result = fixed_t(0);
        } else {
            expected_result = dividend / divisor;
            if      (expected_result < FixedPointMin) expected_result = FixedPointMin;
            else if (expected_result > FixedPointMax) expected_result = FixedPointMax;
        }

        // Pack into AXI word
        axis_in_t in_val;
        in_val.data.range(7,0)   = *reinterpret_cast<ap_int<8>*>(&dividend);
        in_val.data.range(15,8)  = *reinterpret_cast<ap_int<8>*>(&divisor);
        in_val.last = 1;
        in_stream.write(in_val);

        // Save expected result in same order as inputs
        dividends.push_back(dividend);
        divisors.push_back(divisor);
        rand_results.push_back(expected_result);
    }

    // Drain results
    run_and_drain(in_stream, out_stream, num_tests + 20, rand_outputs);

    // Compare
    for (int i = 0; i < num_tests; i++) {
        std::cout << "TEST " << i+1 << ": ";
        if (rand_outputs[i] != rand_results[i]) {
            std::cout << "  NOT EQUAL\n";
            std::cout << "  Dividend: " << dividends[i]    << "\n";
            std::cout << "  Divisor:  " << divisors[i]     << "\n";
            std::cout << "  Result:   " << rand_outputs[i] << "\n";
            std::cout << "  Expected: " << rand_results[i] << "\n";
        } else {
            std::cout << "  PASS\n";
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
        {-14.875, -0.875, 15.875, "Both negative, result is FixedPointMax"}
    };

    // Send inputs
    std::vector<fixed_t> spec_expected;
    for (auto &tc : tests) {
        axis_in_t in_val;
        in_val.data.range(7,0)   = *reinterpret_cast<ap_int<8>*>(&tc.dividend);
        in_val.data.range(15,8)  = *reinterpret_cast<ap_int<8>*>(&tc.divisor);
        in_val.last = 1;
        in_stream.write(in_val);
        spec_expected.push_back(tc.expected_result);
    }

    // Drain
    std::vector<fixed_t> spec_outputs;
    run_and_drain(in_stream, out_stream, tests.size() + 20, spec_outputs);

    // Compare
    for (size_t i = 0; i < tests.size(); i++) {
        std::cout << "TEST " << i+1 << ": ";
        if (spec_outputs[i] != spec_expected[i]) {
            std::cout << "  NOT EQUAL\n";
            std::cout <<                   tests[i].description << "\n";
            std::cout << "  Dividend  " << tests[i].dividend    << "\n";
            std::cout << "  Divisor   " << tests[i].divisor     << "\n";
            std::cout << "  Result:   " << spec_outputs[i]      << "\n";
            std::cout << "  Expected: " << spec_expected[i]     << "\n";
        } else {
            std::cout << "  PASS\n";
        }
    }
    std::cout << std::endl;

    return 0;
}
