#include "arithmetics.h"

uint64_t Arithmetics::mod_pow(uint64_t base, uint64_t exp, uint64_t mod)
{
    uint64_t result = 1;
    base %= mod;

    if (base == 0) return 0;

    while (exp > 0)
    {
        // Check if LSB is 1, if true, multiply result by base
        if (exp & 1) result = ((result % mod) * (base % mod)) % mod;
        
        // Shift to next bit - loop will end when there are no more bits to process
        exp = exp >> 1;

        // Square the base to be used in next iteration
        base = ((base % mod) * (base % mod)) % mod;

    }
    
    return result;
}