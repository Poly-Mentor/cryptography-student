#include "arithmetics.h"

uint64_t Arithmetics::mod_pow(uint64_t base, uint64_t exp, uint64_t mod)
{
    if (mod == 0) throw std::invalid_argument("mod must be > 0");

    uint64_t result = 1;
    base %= mod;
    if (base == 0) return 0;

    while (exp > 0)
    {
        if (exp & 1) result = Arithmetics::mul_mod(result, base, mod);
        exp = exp >> 1;
        base = Arithmetics::mul_mod(base, base, mod);
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

bool Arithmetics::is_probable_prime(uint64_t n)
{
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0) return false;

    // for 64-bit numbers, we can use a fixed set of bases that are sufficient to determine primality with certainty
    std::vector<uint64_t> bases = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41};

    // for bigger numbers, we would need to use random bases and perform multiple rounds of testing to reduce the probability of a false positive

    for (int i = 0; i < bases.size(); i++)
    {
        if (bases[i] >= n) break; // no need to test bases greater than or equal to n
        if (!miller_rabin_pass(n, bases[i])) return false; // if any base fails the test, n is composite
    }

    return true;
}

bool Arithmetics::miller_rabin_pass(uint64_t n, uint64_t a)
{
    uint64_t s = 0;
    uint64_t d = n - 1;

    // Factor n-1 as d * 2^s
    while ((d & 1) == 0) {
        d >>= 1;
        s++;
    }

    // Compute a^d mod n
    uint64_t x = mod_pow(a, d, n);
    // If x is 1 or n-1, n is likely prime for this base
    if (x == 1 || x == n - 1) return true;

    // Square x repeatedly and check if it becomes n-1
    for (uint64_t r = 1; r < s; r++) {
        x = mod_pow(x, 2, n);
        if (x == n - 1) return true; // n is likely prime for this base
    }

    return false;
}

uint64_t Arithmetics::mul_mod(uint64_t a, uint64_t b, uint64_t mod)
{
    return static_cast<uint64_t>((static_cast<unsigned __int128>(a) * b) % mod);
}
