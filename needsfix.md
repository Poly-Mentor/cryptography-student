# Issues to Fix (from PR Code Review)

The following issues were identified during the code review of the Blowfish initial implementation PR and need to be addressed.

---

## 1. `src/blowfish/blowfish.cpp` – `pkcs7Unpad()` underflow / out-of-bounds read (lines 216–220)

**File:** `src/blowfish/blowfish.cpp`

`pkcs7Unpad` computes `input.size() - padLen` before checking `input.size() >= padLen`. If `padLen > input.size()`, the subtraction underflows (`size_t`) and the loop reads out of bounds. Validate `input.size() >= padLen` (and `input.size() % 8 == 0`) before doing any index arithmetic or iterating over the padding bytes.

**Suggested fix:**
```cpp
// Ensure the input is large enough and aligned to the Blowfish block size before accessing padding bytes
if (input.size() < static_cast<size_t>(padLen)) {
    throw(std::invalid_argument("Input is too short for the specified padding length"));
}
if (input.size() % 8 != 0) {
    throw(std::invalid_argument("Input length is not a multiple of the block size"));
}

// Last padLen bytes should all be equal to padLen
size_t start = input.size() - static_cast<size_t>(padLen);
for (size_t i = start; i < input.size(); i++) {
    if (input[i] != padLen) throw(std::invalid_argument("Invalid padding bytes"));
}
```

---

## 2. `src/blowfish/blowfish.cpp` – `pkcs7Pad()` unreachable dead branch (lines 199–201)

**File:** `src/blowfish/blowfish.cpp`

In `pkcs7Pad()`, `paddingLength` will always be in the range 1..8 because it is computed as `blockSize - (input.size() % blockSize)`, so the `if (paddingLength == 0)` branch is unreachable. Remove the dead branch (or recompute padding length in a way that can legitimately yield 0) to make the intent clearer.

---

## 3. `src/blowfish/blowfish.cpp` – `decryptBlock()` does not correctly invert `encryptBlock()` (lines 243–253)

**File:** `src/blowfish/blowfish.cpp`

`decryptBlock()` does not match the standard Blowfish decryption structure (encryption rounds with the P-array applied in reverse). The correct implementation is the same Feistel round function but iterating P from 17 down to 2, then applying a final XOR with `P[1]`/`P[0]`.

**Suggested fix:**
```cpp
// 16 rounds of the Feistel network in reverse order
for (int round = 17; round > 1; --round) {
    L ^= P[round];
    R ^= F(L);
    std::swap(L, R);
}

// Undo the final swap and apply the initial P-array transformations
std::swap(L, R);
R ^= P[1];
L ^= P[0];
```

---

## 4. `src/blowfish/blowfish.cpp` – `decryptText()` missing input length validation (lines 52–60)

**File:** `src/blowfish/blowfish.cpp`

`decryptText()` does not validate that `ciphertext.size()` is a multiple of 8 before calling `bytesToBlocks()`. This may throw a lower-level error from a different code path than `decrypt()`. Add an explicit length check and throw a consistent `std::invalid_argument` message.

---

## 5. `src/blowfish/blowfish.cpp` – `blocksToBytes()` missing capacity reservation (lines 139–157)

**File:** `src/blowfish/blowfish.cpp`

`blocksToBytes()` repeatedly `push_back`s into `result` without reserving capacity, causing unnecessary reallocations for larger inputs. Reserve `blocks.size() * 8` before the loop.

**Suggested fix:**
```cpp
result.reserve(blocks.size() * 8);
```

---

## 6. `src/blowfish/blowfish.h` – Constructor docstring mentions non-existent `key_len` parameter (lines 13–15)

**File:** `src/blowfish/blowfish.h`

The constructor docstring mentions a `key_len` parameter, but the constructor signature only takes `const std::vector<uint8_t>& key`. Update the comment to match the actual API.

**Suggested fix:**
```cpp
/**
 * Constructor that takes an encryption key.
 * @param key The encryption key as a vector of bytes (its size must be between 4 and 56 bytes).
```

---

## 7. `src/blowfish/blowfish.h` – Public API doesn't document ECB mode (lines 28–38)

**File:** `src/blowfish/blowfish.h`

The public `encrypt`/`decrypt` and `encryptText`/`decryptText` APIs operate on independent 64-bit blocks with PKCS#7 padding but don't specify a mode of operation (this is effectively ECB). Document the mode explicitly as "ECB with PKCS#7" in the API docstrings, or encode it in the method naming. Consider also adding a safer mode (CBC/CTR with IV) if this code is intended for anything beyond educational use.

---

## 8. `src/blowfish/crypto_bf_bf_pi.h` – Missing include guard (lines 16–20)

**File:** `src/blowfish/crypto_bf_bf_pi.h`

`crypto_bf_bf_pi.h` has no include guard or `#pragma once`. If it is ever included more than once in a single translation unit, `typedef`s / `#define`s / `struct` definitions can cause redefinition errors. Add a guard (or `#pragma once`) at the top.

---

## 9. `src/main.cpp` – Unused variables / parameters causing potential build warnings (lines 8–26)

**File:** `src/main.cpp`

`CipherAlgorithm`, `CipherMode`, and the `argc`/`argv` parameters are currently unused, which may trigger compiler warnings (or break builds if `-Werror` is used). Remove them, or mark them as intentionally unused (e.g., `(void)argc; (void)argv;`) until CLI parsing is implemented.

---

## 10. `tests/blowfish/test_blowfish.cpp` – Missing `#include <cstdlib>` (lines 1–15)

**File:** `tests/blowfish/test_blowfish.cpp`

The test uses `std::exit(1)` but does not `#include <cstdlib>`. Relying on transitive includes is non-portable. Add `#include <cstdlib>` and consider removing the unused `#include <cstring>`.

---

## 11. `tests/blowfish/test_blowfish.cpp` – `#define private public` is brittle (lines 8–10)

**File:** `tests/blowfish/test_blowfish.cpp`

The `#define private public` trick changes the meaning of the `private` keyword for the included header and is brittle (it can break if `blowfish.h` later includes other headers, or if includes are reordered). Prefer exposing test hooks via a `friend` test helper, a dedicated test-only header, or by testing only through the public API.

---

## 12. `tests/blowfish/test_blowfish.cpp` – Noisy DEBUG prints during normal test execution (lines 95–101)

**File:** `tests/blowfish/test_blowfish.cpp`

The test suite prints conditional `DEBUG:` lines during normal execution, making output noisy and potentially interfering with CI log parsing. Remove these debug prints or gate them behind a compile-time flag that is disabled by default.

---

## 13. `tests/blowfish/test_blowfish.cpp` – Missing regression test for `padLen > input.size()` (lines 60–77)

**File:** `tests/blowfish/test_blowfish.cpp`

Padding validation tests don't cover the case where the last byte indicates a padding length larger than the buffer (e.g., input length 1 with `padLen = 8`). Add a regression test for `padLen > input.size()` to help catch the underflow/OOB class of bugs in `pkcs7Unpad()`.

---

## 14. `tests/tests_readme.md` – "Wymagania" section refers to old flat directory layout (lines 3–11)

**File:** `tests/tests_readme.md`

The "Wymagania" section still refers to the old flat layout (`blowfish.cpp`, `blowfish.h`, `test_blowfish.cpp` in the working directory), but the repo now uses `src/` and `tests/`. Update these lines so the required file paths/directory names are consistent with the actual build command.

**Suggested fix:**
```
Krótka instrukcja budowy i uruchomienia testów jednostkowych dodanych w `tests/blowfish/test_blowfish.cpp`.

Wymagania
- `g++` (kompilator C++ z obsługą C++17)
- katalog roboczy zawierający pliki `src/blowfish/blowfish.cpp`, `src/blowfish/blowfish.h`, `src/blowfish/crypto_bf_bf_pi.h` i `tests/blowfish/test_blowfish.cpp`.

Kompilacja
Utwórz katalog `bin` jeśli go nie ma, a następnie skompiluj (nowa struktura projektu używa `src/` i `tests/`):
```

---

## 15. `tests/tests_readme.md` – Raw `g++` command not in a fenced code block (line 9)

**File:** `tests/tests_readme.md`

There is a raw `g++ ...` command at the top level (line 9) that is not in a fenced code block and appears to be a leftover from an earlier project layout. Remove it or move it under the proper "Kompilacja" section inside the existing `bash` block.
