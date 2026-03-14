#include <iostream>
#include <string>
#include <cstdint>
#include <cmath>

using namespace std;

class Blowfish {

    public:
        /** Constructor that takes a key and its length
         * @param key The encryption key
         * @param key_len The length of the key in bytes (must be between 4 and 56 bytes)
         */
        Blowfish(const uint8_t* key, size_t key_len);

        string encrypt(const string& plaintext);
        string decrypt(const string& ciphertext);

    private:
        void initialize(const uint8_t* key, size_t key_len);
    
        // key-initialized P-array and S-boxes
        uint32_t P[18];
        uint32_t S[4][256];
};