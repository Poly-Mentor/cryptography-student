# C++ implementations of selected cryptographic algorithms

This repository contains solution of a task from school's *Cryptography* course: implement Blowfish, RSA and MD5 algorithms in technology of choice.

C++ was chosen for this project as a balance between performance, control over memory and ease of use. The implementations are designed to be educational and straightforward, rather than optimized for production use, therefore they are not secure, and should not be used in real applications.

## CLI tool usage

The project includes a simple command-line interface (CLI) tool that allows users to interact with the implemented algorithms. The CLI provides options to encrypt and decrypt text or files using Blowfish, generate RSA keys, encrypt and decrypt text/files using RSA, and compute MD5 hashes for text/file inputs.

There is a possibility to use standard input and output for text and file content, which allows for flexible usage in various scenarios, such as piping data between commands or redirecting output to files.

```bash
./bin/cryptotool
Usage: cryptotool ALGORITHM [ARGS]...

ALGORITHM:
  blowfish    Blowfish encryption and decryption
  rsa         RSA encryption and decryption (small educational version)
  md5         MD5 hashing

ARGS:
  For blowfish:
	-e    --encrypt       encryption mode
	-d    --decrypt       decryption mode
	-t    --text  TEXT    input text string
	-f    --file  FILE    input file path
	-o    --output FILE   output file path (optional, if not specified, output will be printed to stdout)
	-k    --key    KEY    key string for encryption/decryption
	-kf   --key-file FILE text file containing key string for encryption/decryption 

  For rsa:
	-l  --key-length N       key length in bits (only values from 9 to 48 bits are supported for educational purposes, not secure)
	-gk --generate-keys  PATH  generate RSA key pair and save to specified path
	-gk --generate-keys     (with no arguments) generate RSA key pair and output it to stdout

	-e  --encrypt        	encryption mode
	-d  --decrypt        	decryption mode
	-t  --text   TEXT    	input text string
	-f  --file   FILE    	input file path
	-o  --output FILE    	output file path (optional, if not specified, output will be printed to stdout)
	-pk --public-key FILE  text file containing public key for encryption
	-sk --private-key FILE 	text file containing private key for decryption

  For md5:
	-f --file   FILE    	input file path
	-t --text   TEXT    	input text string
	-o --output FILE    	output file path (optional, if not specified, output will be printed to stdout)

```

### Examples

#### Blowfish

Generate ciphertext from plaintext 

```bash
# Encrypt "hello world" using Blowfish and output to stdout
./bin/cryptotool blowfish --encrypt --text "hello world" --key "my secret key"

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
./bin/cryptotool rsa -e -f image.png -pk rsa_keys.txt -o ciphertext.bin
```

Decrypt a file using RSA private key

```bash
# Decrypt ciphertext.bin using RSA and save to decrypted.png
./bin/cryptotool rsa -d -f ciphertext.bin -sk rsa_keys.txt -o decrypted.png
```


Decrypt ciphertext using RSA private key

```bash
# Decrypt ciphertext.txt using RSA and output to stdout
./bin/cryptotool rsa -d -f ciphertext.txt -sk rsa_keys.txt
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
.bin/cryptotool md5 -f image.png > image_md5.txt
```


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