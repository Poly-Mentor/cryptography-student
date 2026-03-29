#include <iostream>
#include <cstdint>
#include <string>

#include "../../src/rsa-small/rng.h"

static void fail(const std::string &msg) {
    std::cout << "FAIL: " << msg << "\n";
    std::exit(1);
}

static void pass(const std::string &msg) {
    std::cout << "PASS: " << msg << "\n";
}

int main() {
    RNG rng;

    // Test 1: get_random_odd generates numbers of correct bit length and that are odd
    try {
        for (uint bit_len = 4; bit_len <= 64; bit_len++) {
            uint64_t r = rng.get_random_odd(bit_len);
            // Check that the highest bit is set (ensuring correct bit length) and that the number is odd
            if (r >> (bit_len - 1) == 0) fail("get_random_odd generated a number with incorrect bit length for bit_len=" + std::to_string(bit_len));
            if ((r & 1) == 0) fail("get_random_odd generated an even number for bit_len=" + std::to_string(bit_len));
        }
        pass("get_random_odd generates odd numbers of correct bit length");
    } catch (const std::exception &e) {
        fail(std::string("get_random_odd test threw unexpected exception: ") + e.what());
    }

    // Test 2: get_random_odd throws for bit lengths greater than 64
    try {
        try {
            rng.get_random_odd(65);
            fail("get_random_odd did not throw for bit_len=65");
        } catch (const std::invalid_argument&) {
            // expected
        }
        pass("get_random_odd throws for bit lengths greater than 64");
    } catch (const std::exception &e) {
        fail(std::string("get_random_odd exception test threw unexpected exception: ") + e.what());
    }
    return 0;
}