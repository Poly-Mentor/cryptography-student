#include <iostream>
#include <cstdint>
#include <string>

#include "../../src/rsa-small/rsa-small.h"


static void fail(const std::string &msg) {
    std::cout << "FAIL: " << msg << "\n";
    std::exit(1);
}

static void pass(const std::string &msg) {
    std::cout << "PASS: " << msg << "\n";
}



int main() {

    std::vector<uint8_t> binary_data = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
    std::string text_input = "Hello, world!";

    try {
        RSA::generate_keys(8);
        fail("8-bit key generation request should have thrown an exception");
    } catch (const std::invalid_argument&) {
        pass("8-bit key generation request threw expected exception");
    } catch (const std::exception &e) {
        fail(std::string("8-bit key generation request threw unexpected exception: ") + e.what());
    }

    try {
        RSA::generate_keys(49);
        fail("49-bit key generation request should have thrown an exception");
    } catch (const std::invalid_argument&) {
        pass("49-bit key generation request threw expected exception");
    } catch (const std::exception &e) {
        fail(std::string("49-bit key generation request threw unexpected exception: ") + e.what());
    }
    
    for (uint8_t key_len = 9; key_len <= 48; key_len++) {

        keyPair keys = RSA::generate_keys(key_len);
        std::vector<uint64_t> ciphertext = RSA::encrypt(binary_data, keys.publicKey);
        std::vector<uint8_t> decrypted_binary = RSA::decrypt(ciphertext, keys.privateKey);

        std::string test_case_name = "RSA binary data encryption/decryption with " + std::to_string(key_len) + "-bit keys";
        if (decrypted_binary == binary_data) pass(test_case_name);
        else fail(test_case_name + ": decrypted data does not match original plaintext \nOriginal: " + std::string(binary_data.begin(), binary_data.end()) + "\nDecrypted: " + std::string(decrypted_binary.begin(), decrypted_binary.end()));

        ciphertext = RSA::encryptText(text_input, keys.publicKey);
        std::string decrypted_text = RSA::decryptText(ciphertext, keys.privateKey);

        test_case_name = "RSA text encryption/decryption with " + std::to_string(key_len) + "-bit keys";
        if (decrypted_binary == binary_data) pass(test_case_name);
        else fail(test_case_name + ": decrypted data does not match original plaintext \nOriginal: " + text_input + "\nDecrypted: " + decrypted_text);
    }

}