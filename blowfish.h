#include <iostream>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <crypto_bf_bf_pi.h>

class Blowfish {

    public:
        /** Constructor that takes a key and its length
         * @param key The encryption key
         * @param key_len The length of the key in bytes (must be between 4 and 56 bytes)
         */
        Blowfish(const uint8_t* key, size_t key_len);

        std::string encryptText(const std::string& plaintext);
        std::string decryptText(const std::string& ciphertext);

    private:

        /**
         * Procedure for initializing the P-array and S-boxes with the provided key.
         * This involves:
         * 1. Initializing the P-array and S-boxes with the hexadecimal digits of pi (as per the Blowfish specification).
         * 2. XORing the key with the P-array. The key is repeated as necessary to fill the entire P-array.
         */
        void initialize(const uint8_t* key, size_t key_len);

        /**
         * Encrypts a single 64-bit block of data (split into two 32-bit halves, L and R) using the Blowfish algorithm.
         * Operations are performed in-place - input values of L and R are modified to contain the encrypted output.
         * @param L The left half of the block (32 bits)
         * @param R The right half of the block (32 bits)
         */
        void encryptBlock(uint32_t& L, uint32_t& R);

        /**
         * Decrypts a single 64-bit block of data (split into two 32-bit halves, L and R) using the Blowfish algorithm.
         * Operations are performed in-place - input values of L and R are modified to contain the decrypted output.
         * @param L The left half of the block (32 bits)
         * @param R The right half of the block (32 bits)
         */
        void decryptBlock(uint32_t& L, uint32_t& R);

        /**
         * F-function used in the Blowfish algorithm, which takes a 32-bit input and produces a 32-bit output based on the S-boxes.
         */
        uint32_t F(uint32_t x);

        static std::vector<uint32_t> splitToBlocks(const std::string &text);
        static std::string joinBlocksToString(const std::vector<uint32_t> &blocks);
        
        // key-initialized P-array and S-boxes
        uint32_t P[18];
        uint32_t S[4][256];
};