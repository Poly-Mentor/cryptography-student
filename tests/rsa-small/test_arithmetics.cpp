#include <iostream>
#include <cstdint>
#include <string>

#include "../../src/rsa-small/arithmetics.h"

static void fail(const std::string &msg) {
    std::cout << "FAIL: " << msg << "\n";
    std::exit(1);
}

static void pass(const std::string &msg) {
    std::cout << "PASS: " << msg << "\n";
}

static void test() {
    uint32_t test_case_1 = Arithmetics::mod_pow(3UL, 4UL, 14UL);
    std::string test_case_name = "mod_pow test 1";
    if (test_case_1 == 11) pass(test_case_name);
    else fail(test_case_name);

    uint32_t test_case_2 = Arithmetics::mod_pow(14UL, 7UL, 15UL);
    test_case_name = "mod_pow test 2";
    if (test_case_2 == 14) pass(test_case_name);
    else fail(test_case_name);

    uint64_t test_case_3 = Arithmetics::mod_pow(8UL, 15UL, 41UL);
    test_case_name = "mod_pow test 3";
    if (test_case_3 == 32) pass(test_case_name);
    else fail(test_case_name);

    uint64_t test_case_4 = Arithmetics::mod_pow(27UL, 2UL, 5UL);
    test_case_name = "mod_pow test 4";
    if (test_case_4 == 4) pass(test_case_name);
    else fail(test_case_name);

    uint16_t test_case_5 = Arithmetics::mod_pow(7UL, 3UL, 6UL);
    test_case_name = "mod_pow test 5";
    if (test_case_5 == 1) pass(test_case_name);
    else fail(test_case_name);
}

int main() {

    test();
    
}
