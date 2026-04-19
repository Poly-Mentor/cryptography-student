#pragma once

#include <cstdint>

#include "arithmetics.h"

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


};

