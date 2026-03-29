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
    if (B == 0) {
        *gcd = A;
        *x = 1;
        *y = 0;
        return;
    }

    uint64_t g;
    int64_t x1, y1;
    egcd(B, A % B, &g, &x1, &y1);
    *gcd = g;
    *x = y1;
    *y = x1 - (int64_t)(A / B) * y1;
}

uint64_t Arithmetics::mod_inv(uint64_t a, uint64_t m)
{
    uint64_t gcd;
    int64_t x, y;
    egcd(a, m, &gcd, &x, &y);
    if (gcd != 1) {
        throw std::invalid_argument("Modular inverse does not exist since gcd(a, m) != 1");
    } else {
        // Normalize x to be in range [0, m-1] using signed arithmetic
        int64_t mm = static_cast<int64_t>(m);
        int64_t xm = x % mm;
        if (xm < 0) xm += mm;
        return static_cast<uint64_t>(xm);
    }
    
    return 0;
}
