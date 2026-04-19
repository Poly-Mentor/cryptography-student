#include "rsa-small.h"

keyPair RSA::generate_keys(uint8_t bit_length)
{
    throw std::runtime_error("generate_keys not implemented yet");
    return keyPair();
}

uint64_t RSA::generate_prime(uint8_t bit_length)
{
    RNG rng;
    uint64_t prime_candidate;
    
    do {
         prime_candidate = rng.get_random_odd(bit_length);

    } while (not Arithmetics::is_probable_prime(prime_candidate));

    return prime_candidate;
}
