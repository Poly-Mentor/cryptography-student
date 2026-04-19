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

static void test_mod_pow() {
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

static void test_egcd() {
    uint64_t gcd;
    int64_t x, y;

    Arithmetics::egcd(30UL, 21UL, &gcd, &x, &y);
    std::string test_case_name = "egcd test 1";
    if (gcd == 3 && 30*x + 21*y == gcd) pass(test_case_name);
    else fail(test_case_name);

    Arithmetics::egcd(101UL, 10UL, &gcd, &x, &y);
    test_case_name = "egcd test 2";
    if (gcd == 1 && 101*x + 10*y == gcd) pass(test_case_name);
    else fail(test_case_name);

    Arithmetics::egcd(365UL, 82UL, &gcd, &x, &y);
    test_case_name = "egcd test 3";
    if (gcd == 1 && 365*x + 82*y == gcd) pass(test_case_name);
    else fail(test_case_name);

    Arithmetics::egcd(26UL, 418UL, &gcd, &x, &y);
    test_case_name = "egcd test 4";
    if (gcd == 2 && 26*x + 418*y == gcd) pass(test_case_name);
    else fail(test_case_name);
}

static void test_mod_inv() {
    uint64_t test_case_1 = Arithmetics::mod_inv(3UL, 11UL);
    std::string test_case_name = "mod_inv test 1";
    if (test_case_1 == 4) pass(test_case_name);
    else fail(test_case_name);

    uint64_t test_case_2 = Arithmetics::mod_inv(10UL, 17UL);
    test_case_name = "mod_inv test 2";
    if (test_case_2 == 12) pass(test_case_name);
    else fail(test_case_name);

    uint64_t test_case_3 = Arithmetics::mod_inv(7UL, 26UL);
    test_case_name = "mod_inv test 3";
    if (test_case_3 == 15) pass(test_case_name);
    else fail(test_case_name);

    try {
        Arithmetics::mod_inv(6UL, 12UL);
        fail("mod_inv test 4 should have thrown an exception");
    } catch (const std::invalid_argument&) {
        pass("mod_inv test 4");
    } catch (const std::exception &e) {
        fail(std::string("mod_inv test 4 threw unexpected exception: ") + e.what());
    }
}

static void test_is_probable_prime() {
    struct TestCase {
        uint64_t n;
        bool expected;
    };

    std::vector<TestCase> test_cases = {
        {2UL, true},
        {3UL, true},
        {4UL, false},
        {5UL, true},
        {15UL, false},
        {17UL, true},
        {19UL, true},
        {20UL, false},
        {3132614951UL, true}, // 32-bit prime
        {3132614953UL, false}, // 32-bit composite
        {2322321709UL, true}, // 32-bit prime
        {2322321711UL, false}, // 32-bit composite
        {10645068389063303023UL, true}, // 64-bit prime
        {10645068389063303025UL, false}, // 64-bit composite
        {17970697029717014747UL, true}, // 64-bit prime
        {17970697029717014749UL, false} // 64-bit composite
    };

    for (const auto& test_case : test_cases) {
        bool result = Arithmetics::is_probable_prime(test_case.n);
        std::string test_case_name = "is_probable_prime test for n=" + std::to_string(test_case.n);
        if (result == test_case.expected) pass(test_case_name);
        else fail(test_case_name);
    }
}

int main() {

    test_mod_pow();
    test_egcd();
    test_mod_inv();
    test_is_probable_prime();

}
