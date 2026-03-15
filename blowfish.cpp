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

}


uint32_t Blowfish::F(uint32_t x) {
    // Split the 32-bit input into four 8-bit segments
    uint8_t xarr[4] = {
        x >> 24,
        x >> 16,
        x >> 8,
        x
    };
    // Use the segments to index into the S-boxes and combine the results
    uint32_t result;
    result = S[0][xarr[0]] + S[1][xarr[1]];
    result ^= S[2][xarr[2]];
    result += S[3][xarr[3]];

    return result;
}


void Blowfish::encryptBlock(uint32_t &L, uint32_t &R) {

    // 16 rounds of the Feistel network
    for (uint8_t round = 0; round < 16; round++){
        L ^= P[round];
        uint32_t t = F(L);
        R ^= t;
        swap(L, R);
    }

    // Undo the final swap and apply the final P-array transformations
    swap(L, R);
    R ^= P[16];
    L ^= P[17];
}
