# C++ implementations of selected cryptographic algorithms

This repository contains solution of a task from school's *Cryptography* course: implement Blowfish, RSA and MD5 algorithms in technology of choice.

C++ was chosen for this project as a balance between performance, control over memory and ease of use. The implementations are designed to be educational and straightforward, rather than optimized for production use, therefore they are not secure, and should not be used in real applications.

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