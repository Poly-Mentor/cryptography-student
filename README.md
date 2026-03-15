# C++ implementations of selected cryptographic algorithms

This repository contains solution of a task from school's *Cryptography* course: implement Blowfish, RSA and MD5 algorithms in technology of choice.

C++ was chosen for this project as a balance between performance, control over memory and ease of use. The implementations are designed to be educational and straightforward, rather than optimized for production use.

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