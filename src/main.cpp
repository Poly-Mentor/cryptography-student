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

    bool verbose = false;

    CLI::App main_app{"Cryptography Student Tool - implementations of Blowfish, RSA, and MD5 for educational purposes with a simple CLI interface."};
    main_app.add_flag("-v,--verbose", verbose, "Enable verbose output for debugging");
    CLI::App* blowfish_app = main_app.add_subcommand("blowfish", "Blowfish encryption and decryption");
    // TODO: configure blowfish_app with options for key, mode (encrypt/decrypt), input file, output file, etc.
    CLI::App* rsa_app = main_app.add_subcommand("rsa", "RSA encryption and decryption");
    // TODO: configure rsa_app with options for key generation, encryption/decryption, input file, output file, etc.
    CLI::App* md5_app = main_app.add_subcommand("md5", "MD5 hashing");

    // *** MD5 config ***

    // Create option group for input to visually group related options and help manage mutually exclusive options
    // User must provide EITHER a file OR text input, but not both
    auto* input_group = md5_app->add_option_group("input", "Input source (choose one)");
    std::filesystem::path inputFilePath;
    std::string inputText;
    
    // Add file input option with validation that the file must exist
    // CLI::ExistingFile is a built-in validator that checks if the path points to an existing file
    auto* file_opt = input_group->add_option("-f,--file", inputFilePath, "Input file path for hashing")
        ->check(CLI::ExistingFile);
    
    // Add text input option (no validation needed for string input)
    auto* text_opt = input_group->add_option("-t,--text", inputText, "Input string to hash");
    
    // Make the file and text options mutually exclusive using excludes()
    // This means if user provides --file, they cannot provide --text, and vice versa
    file_opt->excludes(text_opt);
    text_opt->excludes(file_opt);
    
    // Add optional output file path (no validation needed - file doesn't need to exist beforehand)
    std::filesystem::path outputFilePath;
    md5_app->add_option("-o,--output", outputFilePath, "Output file for hash result (optional, prints to stdout if not provided)")->required(false);

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

    // *** Blowfish config ***

    auto* blowfish_mode_group = blowfish_app->add_option_group("mode", "Blowfish operation mode (encrypt or decrypt)");
    CipherMode blowfish_mode;

    auto* enc_flag = blowfish_mode_group->add_flag_callback("-e,--encrypt", [&blowfish_mode]() {
        blowfish_mode = CipherMode::ENCRYPT;
    }, "Encrypt mode");

    auto* dec_flag = blowfish_mode_group->add_flag_callback("-d,--decrypt", [&blowfish_mode]() {
        blowfish_mode = CipherMode::DECRYPT;
    }, "Decrypt mode");

    enc_flag->excludes(dec_flag);
    dec_flag->excludes(enc_flag);

    auto* blowfish_key_group = blowfish_app->add_option_group("key", "Blowfish key input (choose one)");
    std::string blowfish_key;
    std::filesystem::path blowfish_key_path;
    auto* blowfish_key_option = blowfish_key_group->add_option("-k,--key", blowfish_key, "Blowfish key as a string")->expected(1);
    auto* blowfish_key_file_option = blowfish_key_group->add_option("-K,--key-file", blowfish_key_path, "Path to file containing Blowfish key")->check(CLI::ExistingFile)->expected(1);

    blowfish_key_option->excludes(blowfish_key_file_option);
    blowfish_key_file_option->excludes(blowfish_key_option);

    auto* blowfish_input_group = blowfish_app->add_option_group("input", "Input source for Blowfish (choose one)");
    std::string blowfish_input_text;
    std::filesystem::path blowfish_input_file;
    auto* blowfish_text_option = blowfish_input_group->add_option("-t,--text", blowfish_input_text, "Input text string for encryption/decryption");
    auto* blowfish_file_option = blowfish_input_group->add_option("-f,--file", blowfish_input_file, "Input file path for encryption/decryption")->check(CLI::ExistingFile);

    blowfish_text_option->excludes(blowfish_file_option);
    blowfish_file_option->excludes(blowfish_text_option);


    std::filesystem::path blowfish_output_file_path;
    blowfish_app->add_option("-o,--output", blowfish_output_file_path, "Output file path for encrypted/decrypted result (optional, prints to stdout if not provided)")->expected(0, 1);

    blowfish_app->callback([&blowfish_mode, &blowfish_input_text, &blowfish_input_file, &blowfish_output_file_path, &blowfish_key, &blowfish_key_path, verbose]() {

        // Setup key
        if (!blowfish_key.empty() && !blowfish_key_path.empty()) {
            throw CLI::ValidationError("blowfish", "You must provide either --key or --key-file, not both");
        }
        else if (blowfish_key.empty() && blowfish_key_path.empty()) {
            throw CLI::ValidationError("blowfish", "You must provide a key using either --key or --key-file");
        }

        if (!blowfish_key.empty()) {
            if (verbose)
                std::cout << "Using Blowfish key from command line input" << std::endl;
        } else if (!blowfish_key_path.empty()) {
            try {
                File key_file(blowfish_key_path);
                const std::vector<uint8_t>& key_content = key_file.getContentBytes();
                blowfish_key = std::string(key_content.begin(), key_content.end());
                if (verbose)
                    std::cout << "Using Blowfish key from file: " << blowfish_key_path << std::endl;
            } catch (const std::exception& e) {
                throw CLI::ValidationError("blowfish", std::string("Error reading key file: ") + e.what());
            }
        }

        Blowfish bf(blowfish_key);


        bool to_file = not blowfish_output_file_path.empty();

        // Use unique_ptr for output file to ensure proper cleanup and avoid memory leaks, even if exceptions are thrown
        std::unique_ptr<File> output_file{nullptr};
        
        if (to_file) {
            if (verbose)
                std::cout << "Output will be written to file: " << blowfish_output_file_path << std::endl;
                output_file = std::make_unique<File>(blowfish_output_file_path);

        } else {
            if (verbose)
                std::cout << "No output file specified, result will be printed to stdout" << std::endl;
        }
        
        if (blowfish_mode == CipherMode::ENCRYPT) {

            if (verbose)
            std::cout << "Blowfish encryption mode selected" << std::endl;

            if (not blowfish_input_text.empty()){
                if (verbose)
                    std::cout << "Encrypting input text: " << blowfish_input_text << std::endl;

                std::vector<uint8_t> encryptedBytes = bf.encryptText(blowfish_input_text);

                if (to_file) {
                    output_file->saveFileAs(encryptedBytes, blowfish_output_file_path);
                    std::cout << "Encrypted data written to: " << blowfish_output_file_path << std::endl;
                } else {
                    for (uint8_t b : encryptedBytes) {
                        std::cout << std::hex << static_cast<int>(b);
                    }
                    std::cout << std::dec << std::endl;
                }
            }
             else if (not blowfish_input_file.empty()) {

                if (verbose)
                    std::cout << "Encrypting input file: " << blowfish_input_file << std::endl;

                try {
                    File input_file(blowfish_input_file);
                    const std::vector<uint8_t>& file_content = input_file.getContentBytes();
                    std::vector<uint8_t> encryptedBytes = bf.encrypt(file_content);

                    if (to_file) {
                        output_file->saveFileAs(encryptedBytes, blowfish_output_file_path);
                        std::cout << "Encrypted data written to: " << blowfish_output_file_path << std::endl;
                    } else {
                        for (uint8_t b : encryptedBytes) {
                            std::cout << std::hex << static_cast<int>(b);
                        }
                        std::cout << std::dec << std::endl;
                    }
                } catch (const std::exception& e) {
                    throw CLI::ValidationError("blowfish", std::string("Error processing input file: ") + e.what());
                }
            }

        }
        else if (blowfish_mode == CipherMode::DECRYPT) {

            if (verbose)
                std::cout << "Blowfish decryption mode selected" << std::endl;

                if (not blowfish_input_text.empty()){
                    if (verbose)
                        std::cout << "Decrypting input text: " << blowfish_input_text << std::endl;

                    // For decryption, we expect the input text to be a hex string representing the encrypted bytes
                    // We need to convert the hex string back to a vector of bytes before decrypting
                    // TODO: create a utility function for hex string to byte vector conversion
                    std::vector<uint8_t> encryptedBytes;
                    for (size_t i = 0; i < blowfish_input_text.length(); i += 2) {
                        std::string byteString = blowfish_input_text.substr(i, 2);
                        uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
                        encryptedBytes.push_back(byte);
                    }

                    std::vector<uint8_t> decryptedBytes = bf.decrypt(encryptedBytes);
                    std::string decryptedText(decryptedBytes.begin(), decryptedBytes.end());

                    if (to_file) {
                        output_file->saveFileAs(decryptedBytes, blowfish_output_file_path);
                        std::cout << "Decrypted data written to: " << blowfish_output_file_path << std::endl;
                    } else {
                        std::cout << "Decrypted text: " << decryptedText << std::endl;
                    }
                }
                 else if (not blowfish_input_file.empty()) {

                    if (verbose)
                        std::cout << "Decrypting input file: " << blowfish_input_file << std::endl;

                    try {
                        File input_file(blowfish_input_file);
                        const std::vector<uint8_t>& file_content = input_file.getContentBytes();
                        std::vector<uint8_t> decryptedBytes = bf.decrypt(file_content);
                        std::string decryptedText(decryptedBytes.begin(), decryptedBytes.end());

                        if (to_file) {
                            output_file->saveFileAs(decryptedBytes, blowfish_output_file_path);
                            std::cout << "Decrypted data written to: " << blowfish_output_file_path << std::endl;
                        } else {
                            std::cout << "Decrypted text: " << decryptedText << std::endl;
                        }
                    } catch (const std::exception& e) {
                        throw CLI::ValidationError("blowfish", std::string("Error processing input file: ") + e.what());
                    }
                }
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
