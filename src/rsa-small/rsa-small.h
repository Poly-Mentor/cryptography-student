#pragma once

#include <cstdint>
#include <stdexcept>

#include "arithmetics.h"
#include "rng.h"

struct key
{
    uint64_t exponent;
    uint64_t modulus;
};

struct keyPair
{
    key publicKey;
    key privateKey;
};



class RSA
{

public:
    keyPair generate_keys(uint8_t bit_length);

private:
    /**
     * Generate a prime number of the specified bit length using the RNG and primality testing.
     */
    uint64_t generate_prime(uint8_t bit_length);

};

