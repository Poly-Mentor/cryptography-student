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

    CLI::App main_app{"Cryptography Student Tool - implementations of Blowfish, RSA, and MD5 for educational purposes with a simple CLI interface."};
    CLI::App* blowfish_app = main_app.add_subcommand("blowfish", "Blowfish encryption and decryption");
    // TODO: configure blowfish_app with options for key, mode (encrypt/decrypt), input file, output file, etc.
    CLI::App* rsa_app = main_app.add_subcommand("rsa", "RSA encryption and decryption");
    // TODO: configure rsa_app with options for key generation, encryption/decryption, input file, output file, etc.
    CLI::App* md5_app = main_app.add_subcommand("md5", "MD5 hashing");

    // Create option group for input to visually group related options and help manage mutually exclusive options
    // User must provide EITHER a file OR text input, but not both
    auto* input_group = md5_app->add_option_group("input", "Input source (choose one)");
    std::filesystem::path inputFilePath;
    std::string inputText;
    
    // Add file input option with validation that the file must exist
    // CLI::ExistingFile is a built-in validator that checks if the path points to an existing file
    auto* file_opt = input_group->add_option("-f,--file", inputFilePath, "Input file to hash")
        ->check(CLI::ExistingFile);
    
    // Add text input option (no validation needed for string input)
    auto* text_opt = input_group->add_option("-t,--text", inputText, "Input string to hash");
    
    // Make the file and text options mutually exclusive using excludes()
    // This means if user provides --file, they cannot provide --text, and vice versa
    file_opt->excludes(text_opt);
    text_opt->excludes(file_opt);
    
    // Add optional output file path (no validation needed - file doesn't need to exist beforehand)
    std::filesystem::path outputFilePath;
    md5_app->add_option("-o,--output", outputFilePath, "Output file for hash result (optional, prints to stdout if not provided)");

    // Set a callback function that runs when the md5 subcommand is invoked
    // This is where we define the actual behavior of the md5 command
    md5_app->callback([&inputFilePath, &inputText, &outputFilePath]() {
        // Validate that exactly one input source was provided
        // This is a safety check in addition to CLI11's excludes() mechanism
        if (inputFilePath.empty() && inputText.empty()) {
            throw CLI::ValidationError("md5", "You must provide either --file or --text");
        }
        
        // Compute MD5 hash based on which input source was provided
        std::string hash_result;
        
        if (!inputFilePath.empty()) {
            // File input path: read file and compute hash using the File utility class
            try {
                // Create File object - validates that file exists (throws if not)
                File file(inputFilePath);
                
                // Get file content as bytes (lazy-loaded and cached)
                const std::vector<uint8_t>& file_content = file.getContentBytes();
                
                // Compute MD5 hash of the file content
                MD5 md5;
                hash_result = md5.calculate(file_content);
            } catch (const std::exception& e) {
                throw CLI::ValidationError("md5", std::string("Error reading file: ") + e.what());
            }
        } else if (!inputText.empty()) {
            // Text input: compute hash of the string directly
            // MD5::calculate() has an overload that accepts std::string directly
            MD5 md5;
            hash_result = md5.calculate(inputText);
        }
        
        // Handle output: either write to file or print to stdout
        if (!outputFilePath.empty()) {
            // Write hash result to output file
            try {
                std::ofstream output_file(outputFilePath);
                if (!output_file) {
                    throw CLI::ValidationError("md5", "Cannot create output file: " + outputFilePath.string());
                }
                output_file << hash_result;
                output_file.close();
                std::cout << "MD5 hash written to: " << outputFilePath << std::endl;
            } catch (const std::exception& e) {
                throw CLI::ValidationError("md5", std::string("Error writing to output file: ") + e.what());
            }
        } else {
            // Print hash to stdout (default behavior if no output file specified)
            std::cout << hash_result << std::endl;
        }
    });

    CLI11_PARSE(main_app, argc, argv);

    //blowfishTextDemo();

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
