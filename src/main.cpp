#include <cstdint>
#include <iostream>
#include <vector>
#include <fstream>
#include <stdexcept>

#include "utils/utils.h"
#include "utils/CLI11.hpp"
#include "blowfish/blowfish.h"
#include "rsa-small/rsa-small.h"
#include "md5/md5.h"

enum class CipherAlgorithm {
    Blowfish,
    RSA,
    MD5
};

enum class CipherMode {
    ENCRYPT,
    DECRYPT
};

void blowfishTextDemo();

int main(int argc, char* argv[]) {

    blowfishTextDemo();

    return 0;
}

void blowfishTextDemo() {

    using namespace std;

    string key = "testKey12345678"; // Blowfish key must be between 4 and 56 bytes
    const string input = "Hello, World!";

    cout << "Generating Blowfish cipher with key: " << key << endl;
    Blowfish bf = Blowfish(key);

    cout << "Encrypting text: " << input << endl;
    vector<uint8_t> encryptedBytes = bf.encryptText(input);

    cout << "Encrypted bytes: ";
    for (uint8_t b : encryptedBytes) {
        cout << hex << static_cast<int>(b) << " ";
    }
    cout << dec << endl;

    string decryptedText = bf.decryptText(encryptedBytes);
    cout << "Decrypted text: " << decryptedText << endl;
}
