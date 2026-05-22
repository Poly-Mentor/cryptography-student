# C++ implementations of selected cryptographic algorithms

This repository contains solution of a task from school's *Cryptography* course: implement Blowfish, RSA and MD5 algorithms in technology of choice.

C++ was chosen for this project as a balance between performance, control over memory and ease of use. The implementations are designed to be educational and straightforward, rather than optimized for production use, therefore they are not secure, and should not be used in real applications.

## CLI tool usage

The project includes a simple command-line interface (CLI) tool that allows users to interact with the implemented algorithms. The CLI provides options to encrypt and decrypt text or files using Blowfish, generate RSA keys, encrypt and decrypt text/files using RSA, and compute MD5 hashes for text/file inputs.

There is a possibility to use standard input and output for text and file content, which allows for flexible usage in various scenarios, such as piping data between commands or redirecting output to files.

```bash
./bin/cryptotool --help
Cryptography Student Tool - implementations of Blowfish, RSA, and MD5 for
educational purposes with a simple CLI interface.


./bin/cryptotool [OPTIONS] [SUBCOMMANDS]


OPTIONS:
  -h,     --help              Print this help message and exit
  -v,     --verbose           Enable verbose output for debugging

SUBCOMMANDS:
  blowfish                    Blowfish encryption and decryption
  rsa                         RSA encryption and decryption
  md5                         MD5 hashing

./bin/cryptotool blowfish --help
Blowfish encryption and decryption


./bin/cryptotool blowfish [OPTIONS]


OPTIONS:
  -h,     --help              Print this help message and exit
  -o,     --output TEXT       Output file path for encrypted/decrypted result (optional, prints
                              to stdout if not provided)
[Option Group: mode]
  Blowfish operation mode (encrypt or decrypt)
  
  
OPTIONS:
  -e,     --encrypt Excludes: --decrypt 
                              Encrypt mode
  -d,     --decrypt Excludes: --encrypt 
                              Decrypt mode
[Option Group: key]
  Blowfish key input method
  [At least 1 of the following options are required]
  
  
OPTIONS:
  -k,     --key TEXT Excludes: --key-file 
                              Blowfish key as a string
  -K,     --key-file TEXT:FILE Excludes: --key 
                              Path to file containing Blowfish key
[Option Group: input]
  Input data source
  [At least 1 of the following options are required]
  
  
OPTIONS:
  -t,     --text TEXT Excludes: --file 
                              Input text string for encryption/decryption
  -f,     --file TEXT:FILE Excludes: --text 
                              Input file path for encryption/decryption

./bin/cryptotool md5 --help
MD5 hashing


./bin/cryptotool md5 [OPTIONS]


OPTIONS:
  -h,     --help              Print this help message and exit
  -o,     --output TEXT       Output file for hash result (optional, prints to stdout if not
                              provided)
[Option Group: input]
  Input source (choose one)
  [At least 1 of the following options are required]
  
  
OPTIONS:
  -f,     --file TEXT:FILE Excludes: --text 
                              Input file path for hashing
  -t,     --text TEXT Excludes: --file 
                              Input string to hash
```

### Examples

#### Blowfish

Generate ciphertext from plaintext 

```bash
# Encrypt "hello world" using Blowfish and output to stdout
./bin/cryptotool blowfish --encrypt --text "hello world" --key "my secret key"
# or using path to key file
./bin/cryptotool blowfish -e -t "hello world" -K key.txt

# Encrypt "hello world" using Blowfish and save to ciphertext.txt
./bin/cryptotool blowfish -e -t "hello world" -k "my secret key" > ciphertext.txt
# or
./bin/cryptotool blowfish -e -t "hello world" -k "my secret key" -o ciphertext.txt
```

Encrypt a file and save the ciphertext to another file

```bash
# Encrypt input.txt using Blowfish and save to ciphertext.bin
./bin/cryptotool blowfish -e -f input.txt -k "my secret key" -o ciphertext.bin
# of course, file can be any type, not just text
./bin/cryptotool blowfish -e -f image.png -k "my secret key" -o image.enc
```

Decrypt the ciphertext back to plaintext

```bash
# Decrypt ciphertext.txt using Blowfish and output to stdout
./bin/cryptotool blowfish --decrypt --file ciphertext.txt --key "my secret key"
# Decrypt ciphertext.bin using Blowfish and save to decrypted.txt
./bin/cryptotool blowfish -d -f ciphertext.bin -k "my secret key" -o decrypted.txt
```

Decrypting a file back to original

```bash
# Decrypt image.enc using Blowfish and save to image_dec.png
./bin/cryptotool blowfish -d -f image.enc -k "my secret key" -o image_dec.png
```

#### RSA

Generate RSA key pair

```bash
# Generate RSA key pair and save to current directory
./bin/cryptotool rsa --generate-keys --output rsa_keys.txt


# Key length can be specified between 9 and 48 bits (for educational purposes, not secure)
./bin/cryptotool rsa -gk -l 32

# Generate RSA key pair and output to stdout
./bin/cryptotool rsa -gk
```

Encrypt plaintext using RSA public key

```bash
# Encrypt "hi" using RSA and save to ciphertext.txt
./bin/cryptotool rsa --encrypt --text "hi" --public-key rsa_public_key.txt --output ciphertext.txt
```

Encrypt a file using RSA public key

```bash
# Encrypt image.png using RSA and save to ciphertext.bin
./bin/cryptotool rsa -e -f image.png -P rsa_keys.txt -o ciphertext.bin
```

Decrypt a file using RSA private key

```bash
# Decrypt ciphertext.bin using RSA and save to decrypted.png
./bin/cryptotool rsa -d -f ciphertext.bin -S rsa_keys.txt -o decrypted.png
```


Decrypt ciphertext using RSA private key

```bash
# Decrypt ciphertext.txt using RSA and output to stdout
./bin/cryptotool rsa -d -f ciphertext.txt -S rsa_keys.txt
```

#### MD5

Compute MD5 hash of a text string. You must pass either a file path or a text string, but not both at the same time.

```bash
# Compute MD5 hash of "hello world" and output to stdout
./bin/cryptotool md5 --text "hello world"
```

Compute MD5 hash of a file

```bash
# Compute MD5 hash of input.txt and output to stdout
./bin/cryptotool md5 --file input.txt

# Compute MD5 hash of image.png and output to a file
./bin/cryptotool md5 --file image.png --output image_md5.txt
# or
./bin/cryptotool md5 -f image.png > image_md5.txt
```


## Web-based Demo

The project includes an interactive browser-based demonstration tool built with WebAssembly (WASM). This allows you to use all three cryptographic algorithms directly in your web browser without any server-side processing.

### Features

- **Interactive UI**: Clean, responsive interface with tab-based navigation for each algorithm
- **MD5 Hashing**: Compute message digests for text input or uploaded files
- **Blowfish Encryption**: Encrypt and decrypt data with password-based keys, supporting both text and files
- **RSA Encryption**: Generate key pairs, encrypt with public keys, and decrypt with private keys
- **File Upload Support**: Upload files directly to be processed by any algorithm
- **File Download**: Download encrypted/decrypted results as files for roundtrip encryption-decryption workflows
- **Copy to Clipboard**: Easily copy results with the copy button
- **Client-side Processing**: All cryptographic operations run in the browser using WebAssembly—no data is sent to any server

### Accessing the Demo

1. Build the WASM module:
```bash
make build_wasm
```

2. Open the demo in your browser:
```bash
# From the repository root, open this file in your browser
file:///path/to/cryptography-student/html-demo/cryptotool.html
```

Alternatively, you can serve the files using a simple HTTP server:
```bash
cd html-demo
python -m http.server 8000
# Then visit http://localhost:8000/cryptotool.html
```

### Building the WASM Module

The web demo requires the WebAssembly module to be built from the C++ source code using Emscripten. The build process is automated through the Makefile:

```bash
# Build the WASM module (creates cryptotool.js and cryptotool.wasm)
make build_wasm
```

The compiled files are placed in `html-demo/` directory:
- `cryptotool.js` - JavaScript loader for the WebAssembly module
- `cryptotool.wasm` - WebAssembly binary containing the compiled C++ cryptographic implementations

### Usage Examples in the Browser

#### MD5 Hashing

1. Navigate to the "MD5 Hash" tab
2. Choose "Text" input for manual text, or "File" to upload a document
3. Enter your text or select a file
4. Click "Compute Hash"
5. Copy the resulting 128-bit hash value

#### Blowfish Encryption

1. Go to the "Blowfish Cipher" tab
2. Enter an encryption key (4-56 bytes)
3. Select "Encrypt" or "Decrypt" mode
4. Choose "Text" or "File" input
5. Enter plaintext or ciphertext (or upload a file)
6. Click "Process" to encrypt/decrypt
7. Copy the result or download it as a file for roundtrip encryption-decryption workflows

**Roundtrip File Encryption Workflow:**
- Encrypt a file: Upload file → Select "Encrypt" → Click "Process" → Click "Download" to save encrypted file
- Decrypt the file: Upload encrypted file → Select "Decrypt" → Click "Process" → Click "Download" to get original file back

#### RSA Encryption

1. Visit the "RSA Encryption" tab
2. Select key size (16, 32, or 48-bit for demonstration)
3. Click "Generate Keys" to create a public/private key pair
4. Copy the keys for use
5. For encryption: paste the public key, enter plaintext (or upload file), select "Encrypt", click "Process"
6. For decryption: paste the private key, enter the comma-separated ciphertext, select "Decrypt", click "Process"
7. Download encrypted or decrypted results using the "Download" button

**Roundtrip File Encryption Workflow:**
- Encrypt a file: Upload file → Select "Encrypt" → Enter public key → Click "Process" → Click "Download"
- Decrypt the file: Upload encrypted file → Select "Decrypt" → Enter private key → Click "Process" → Click "Download"

### Technical Details

- **Framework**: WebAssembly compiled from C++ using Emscripten
- **JavaScript Integration**: Emscripten embind bindings expose C++ classes and functions to JavaScript
- **File Handling**: Browser File API used for reading uploaded files
- **Compatibility**: Works in all modern browsers supporting WebAssembly
- **Security Note**: While client-side processing is used, these are educational implementations and should not be relied upon for real cryptographic security


# Implementation

## Blowfish

The Blowfish implementation is based on the original algorithm description by Bruce Schneier. It includes key expansion and encryption/decryption functions. The key expansion initializes the P-array and S-boxes using the provided key, while the encryption and decryption functions perform the actual data transformation.

Blowfish class is stateful, meaning that it retains the expanded key and S-boxes in memory after initialization. This allows for efficient encryption and decryption of multiple blocks of data without needing to reinitialize the algorithm.

### Example usage

```cpp
// Blowfish roundtrip example
#include <iostream>
#include "blowfish/blowfish.h"

int main() {
	std::string plaintext = "hello world";
	Blowfish bf("my secret key");
	auto ciphertext = bf.encryptText(plaintext);
	auto recovered = bf.decryptText(ciphertext);
	std::cout << recovered << std::endl;
	return 0;
}
```

## RSA

RSA implementation includes key generation, encryption and decryption functions. The key generation function creates a pair of public and private keys based on two prime numbers. The encryption function takes plaintext and a public key to produce ciphertext, while the decryption function uses the private key to retrieve the original plaintext from the ciphertext.

RSA class is stateless, meaning that it does not retain any information about the keys or the state of the algorithm after each operation. Each function call is independent and does not rely on any previous state.

RSA-small implementation is a simplified version of RSA that operates on small integers for educational purposes, as it does not require handling of large numbers and complex padding schemes. It is designed to demonstrate the basic principles of RSA encryption and decryption without the complexities of a full implementation.

### Example usage

```cpp
// rsa-small roundtrip example
#include <iostream>
#include "rsa-small/rsa-small.h"

int main() {
	auto kp = RSA::generate_keys(16); // small educational keys
	std::string plaintext = "hi";
	auto ciphertext = RSA::encryptText(plaintext, kp.publicKey);
	auto recovered = RSA::decryptText(ciphertext, kp.privateKey);
	std::cout << recovered << std::endl;
	return 0;
}
```

## MD5
The MD5 implementation includes a function to compute the MD5 hash of a given input string or binary data in form of a vector of bytes. The algorithm processes the input in 512-bit blocks and produces a 128-bit hash value. The implementation follows the standard MD5 algorithm as defined in RFC 1321.

MD5 class is stateless in a sense that it does not retain any information about the state of the algorithm after each operation. However it does maintain internal state during the computation of the hash, as it processes the input data in blocks and updates the hash value accordingly. Once the final hash is computed, the **internal state is reset** for the next computation. Due to this design, the MD5 class **is not static** - it requires an instance to be created.

### Example usage

```cpp
// MD5 example
#include <iostream>
#include "md5/md5.h"

int main() {
	MD5 md5;
	std::string input = "hello world";
	auto hash = md5.calculate(&input);
	std::cout << hash << std::endl;
	return 0;
}
```


## AI usage disclosure

Generative AI was used for:
- General help with C++ syntax, features and best practices
- Assistance with providing friendly, step-by-step, no-code descriptions of cryptographic algorithms to implement
- Writing tests

Generative AI was **not** used for:
- Writing the actual implementations of the algorithms (in /src)

## Third-party code

- The Blowfish implementation is based on the original algorithm description by Bruce Schneier. It does not directly copy any specific codebase.

- crypto_bf_bf_pi.h is a header file containing the initialization of Blowfish's P-array and S-boxes, which are derived from the hexadecimal digits of pi. This file comes from [OpenSSL](https://github.com/openssl/openssl) project. License of this repository was adopted to match OpenSSL's license (Apache License 2.0) to allow inclusion of this file.

- [CLI11](https://github.com/CLIUtils/CLI11) was used to create Command Line Interface
