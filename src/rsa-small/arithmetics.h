#pragma once

#include <cstdint>
#include <utility>


/** 
 * A class for performing various arithmetic operations used in RSA implementation, such as modular exponentiation, extended GCD, and modular inverse.
 * All operations are designed to work with 64-bit unsigned integers (`uint64_t`), which is sufficient for RSA keys up to 48 bits (p and q up to 24 bits each).
 */
class Arithmetics {

public:

    /**
     * Calculate (base^exp) mod mod using the square-and-multiply algorithm.
     * @param base The base number
     * @param exp The exponent
     * @param mod The modulus
     * @return The result of (base^exp) mod mod
     * @throws std::invalid_argument if mod is zero
     */
    static uint64_t mod_pow(uint64_t base, uint64_t exp, uint64_t mod);

    /**
     * Calculate the greatest common divisor of a and b, along with coefficients x and y such that ax + by = gcd(a, b).
     * @param a First number
     * @param b Second number
     * @param gcd Pointer to store the greatest common divisor
     * @param x Pointer to store the coefficient for a
     * @param y Pointer to store the coefficient for b
     */
    static void egcd(uint64_t a, uint64_t b, uint64_t *gcd, int64_t *x, int64_t *y);

    /**
     * Calculate the modular inverse of a modulo m, i.e., find x such that (a * x) mod m = 1.
     * @param a The number to find the inverse of
     * @param m The modulus
     * @return The modular inverse of a modulo m
     * @throws std::invalid_argument if the modular inverse does not exist (i.e., if gcd(a, m) != 1)
     */
    static uint64_t mod_inv(uint64_t a, uint64_t m);
};
