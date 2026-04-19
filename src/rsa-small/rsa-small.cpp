#include "rsa-small.h"

keyPair RSA::generate_keys(uint8_t bit_length)
{
    
    if (bit_length < 8 || bit_length > 48) {
        throw std::invalid_argument("Bit length must be between 8 and 48");
    }

    uint64_t p = generate_prime(bit_length / 2);
    uint64_t q = generate_prime(bit_length / 2);
    uint64_t n = p * q;
    uint64_t phi = (p - 1) * (q - 1);
    uint64_t e;

    uint8_t phi_bit_length = 0;
    for (uint64_t temp = phi; temp > 0; temp >>= 1) {
        phi_bit_length++;
    }

    RNG rng;
    uint64_t gcd;
    int64_t x, y;
    do {
        e = rng.get_random_odd(phi_bit_length / 2);
        Arithmetics::egcd(e, phi, &gcd, &x, &y);
    } while (e >= phi || gcd != 1);
    // e = 65537; // Common choice for e, and it is coprime to phi for any p and q that are not multiples of 65537
    
    uint64_t d = Arithmetics::mod_inv(e, phi);

    key publicKey = {e, n};
    key privateKey = {d, n};

    return keyPair({publicKey, privateKey});
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
