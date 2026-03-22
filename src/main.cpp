#include <cstdint>
#include <iostream>
#include <vector>
#include <fstream>
#include <stdexcept>
#include "blowfish.h"

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

std::vector<uint8_t> loadFile(std::string filepath) {

    std::ifstream file(filepath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open file: " + filepath);
    }

    // Move the file pointer to the end
    file.seekg(0, std::ios::end);
    // Get the current position, which is the size of the file
    std::streampos sp = file.tellg();
    // Check if tellg() succeeded
    if (sp == std::streampos(-1)) {
        throw std::runtime_error("Failed to determine file size: " + filepath);
    }

    // Convert the position to a size_t for the vector allocation
    std::size_t n = static_cast<std::size_t>(sp);
    // Move the file pointer back to the beginning of the file
    file.seekg(0, std::ios::beg);

    // Initialize a vector with predetermined size
    std::vector<uint8_t> bytes(n);
    if (n > 0) {
        // Read the file content into the vector in one go
        file.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(n));
        // Check how many bytes were actually read
        std::streamsize actuallyRead = file.gcount();
        if (actuallyRead < static_cast<std::streamsize>(n)) {
            bytes.resize(static_cast<std::size_t>(actuallyRead));
        }
    }

    return bytes;
}

void saveFile(std::string filepath, const std::vector<uint8_t>& data) {

    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Could not open file for writing: " + filepath);
    }
    // Write the data to the file in one go
    // file.write() expects a const char* and a streamsize, so we need to reinterpret the data pointer and cast the size
    file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));

    if (!file) {
        throw std::runtime_error("Could not write to file: " + filepath);
    }
}