#include "rsa-small.h"

static const bool verbose = true;

keyPair RSA::generate_keys(uint8_t bit_length)
{

    if (bit_length < 9 || bit_length > 48) {
        throw std::invalid_argument("Bit length must be between 9 and 48");
    }

    uint8_t p_bits = bit_length / 2;
    uint8_t q_bits = bit_length - p_bits;

    if (verbose) {
        std::cout << "Generating p with " << (int)p_bits << " bits...\n";
    }

    uint64_t p = generate_prime(p_bits);
    uint64_t q = generate_prime(q_bits);

    // Ensure p and q are distinct
    while (q == p) {
        q = generate_prime(q_bits);
    }

    if (verbose) {
        std::cout << "Generated primes:\np = " << p << "\nq = " << q << "\n";
    }

    uint64_t n = p * q;
    uint64_t phi = (p - 1) * (q - 1);
    uint64_t e;

    uint8_t phi_bit_length = 0;
    for (uint64_t temp = phi; temp > 0; temp >>= 1) {
        phi_bit_length++;
    }

    RNG rng = RNG();
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

    if (verbose) {
        std::cout << "Generated keys:\nPublic key: (e=" << publicKey.exponent << ", n=" << publicKey.modulus << ")\nPrivate key: (d=" << privateKey.exponent << ", n=" << privateKey.modulus << ")\n";
    }

    return keyPair({publicKey, privateKey});
}

std::vector<uint64_t> RSA::encrypt(const std::vector<uint8_t> &plaintext, const key &publicKey)
{
    std::vector<uint64_t> ciphertext;
    ciphertext.reserve(plaintext.size());

    for (uint8_t byte : plaintext)
    {
        ciphertext.push_back(Arithmetics::mod_pow(static_cast<uint64_t>(byte), publicKey.exponent, publicKey.modulus));
    }
    
    return ciphertext;
}

std::vector<uint8_t> RSA::decrypt(const std::vector<uint64_t> &ciphertext, const key &privateKey)
{
    std::vector<uint8_t> plaintext;
    plaintext.reserve(ciphertext.size());

    for (uint64_t cipher : ciphertext)
    {
        uint64_t decrypted = Arithmetics::mod_pow(cipher, privateKey.exponent, privateKey.modulus);
        if (decrypted > 255) {
            throw std::runtime_error("Decrypted value exceeds byte size, likely due to incorrect key or corrupted ciphertext");
        }
        plaintext.push_back(static_cast<uint8_t>(decrypted));
    }
    
    return plaintext;
}

uint64_t RSA::generate_prime(uint8_t bit_length)
{
    RNG rng = RNG();
    uint64_t prime_candidate;

    do {
         prime_candidate = rng.get_random_odd(bit_length);

    } while (not Arithmetics::is_probable_prime(prime_candidate));

    return prime_candidate;
}
