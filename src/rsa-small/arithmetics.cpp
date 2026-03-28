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


void Arithmetics::egcd(uint64_t A, uint64_t B, uint64_t *gcd, int64_t *x, int64_t *y) {
    if (B > A)
        std::swap(A, B);  // Ensure that A >= B

    if (B == 0) {
        *gcd = A; // GCD is A when B is 0
        *x = 1;
        *y = 0;
    } else {
        egcd(B, A % B, gcd, x, y); // Recursive call with B and A mod B
        int64_t temp = *x; // Update x and y using results from recursive call
        *x = *y;
        *y = temp - (A / B) * (*y); // Update y based on the quotient of A and B
    }
}
