#include "blowfish.h"
#include <crypto_bf_bf_pi.h>

Blowfish::Blowfish(const uint8_t* key, size_t key_len) {
    if (key_len < 4 || key_len > 56) {
        throw invalid_argument("Key length must be between 4 and 56 bytes");
    }
    initialize(key, key_len);
}

void Blowfish::initialize(const uint8_t* key, size_t key_len) {

    // Initialize P-array and S-boxes with initial values
    // those values are derived from the hexadecimal digits of pi (as per the Blowfish specification)
    // provided in crypto_bf_bf_pi.h
    for (int i = 0; i < 18; i++) {
        P[i] = bf_init.P[i];
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 256; j++) {
            S[i][j] = bf_init.S[i * 256 + j];
        }
    }

    // XOR the key with the P-array
    size_t key_index = 0;
    for (int i = 0; i < 18; i++) {
        uint32_t data = 0;
        // Combine 4 bytes of the key into a 32-bit word
        for (int j = 0; j < 4; j++) {
            // Shift the existing data left by 8 bits and add the next byte of the key
            data = (data << 8) | key[key_index];
            // Move to the next byte of the key, wrapping around if necessary
            key_index = (key_index + 1) % key_len;
        }
        P[i] ^= data;
    }

    // TODO

    // // Encrypt the all-zero string to further mix the P-array and S-boxes
    // uint32_t left = 0, right = 0;
    // for (int i = 0; i < 18; i += 2) {
    //     encryptBlock(left, right);
    //     P[i] = left;
    //     P[i + 1] = right;
    // }
    // for (int i = 0; i < 4; i++) {
    //     for (int j = 0; j < 256; j += 2) {
    //         encryptBlock(left, right);
    //         S[i][j] = left;
    //         S[i][j + 1] = right;
    //     }
    // }
}