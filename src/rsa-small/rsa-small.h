#pragma once

#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>

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
    static keyPair generate_keys(uint8_t bit_length);

    // Binary-oriented API
    static std::vector<uint64_t> encrypt(const std::vector<uint8_t>& plaintext, const key& publicKey);
    static std::vector<uint8_t> decrypt(const std::vector<uint64_t>& ciphertext, const key& privateKey);

    // Text-oriented API
    /**
     * Encrypts a plaintext string and returns the ciphertext as a vector of bytes.
     */
    static std::vector<uint64_t> encryptText(const std::string& plaintext, const key& publicKey);

    /**
     * Decrypts a ciphertext represented as a vector of bytes and returns the resulting plaintext string.
     */
    static std::string decryptText(const std::vector<uint64_t>& ciphertext, const key& privateKey);

private:
    /**
     * Generate a prime number of the specified bit length using the RNG and primality testing.
     */
    static uint64_t generate_prime(uint8_t bit_length);

};

