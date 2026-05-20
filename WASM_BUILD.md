# Cryptography Tool - WASM Web Application

## Project Summary

Your cryptography student project has been successfully compiled to WebAssembly (WASM) and is now running as an interactive web application!

## What Was Done

### 1. **Created Emscripten Bindings** ([src/wasm_wrapper.cpp](src/wasm_wrapper.cpp))
   - Created C++ wrapper with embind to expose cryptographic functions to JavaScript
   - Exposed `md5Hash()` function for MD5 hashing
   - Exposed `Blowfish` class for encryption/decryption operations
   - Includes hex conversion utilities for WASM ↔ JavaScript data exchange

### 2. **Built Professional Web Interface** ([bin/cryptotool.html](bin/cryptotool.html))
   - Beautiful, responsive design with gradient background
   - Tab-based interface for different crypto operations
   - Two main features:
     - **MD5 Hash**: Compute hash for any text input
     - **Blowfish Cipher**: Encrypt/decrypt text with a custom key
   - Real-time results display with copy-to-clipboard functionality
   - Mobile-responsive layout

### 3. **Created Build Script** ([build_wasm.sh](build_wasm.sh))
   - Automated WASM compilation using emscripten
   - Compiles all source files with proper C++ flags
   - Generates:
     - `cryptotool.js` - JavaScript loader
     - `cryptotool.wasm` - Compiled binary
     - `cryptotool.html` - Web interface

## Generated Files

- **bin/cryptotool.html** - Main web application (open this in your browser)
- **bin/cryptotool.js** - WASM module loader
- **bin/cryptotool.wasm** - Compiled WebAssembly binary
- **src/wasm_wrapper.cpp** - C++ embind bindings

## How to Use

### Build the WASM Module
```bash
chmod +x build_wasm.sh
./build_wasm.sh
```

### Open the Web App
Open `bin/cryptotool.html` in your web browser (any modern browser supporting WebAssembly)

### Using MD5 Hash Tab
1. Enter text you want to hash
2. Click "Compute Hash"
3. View and copy the 128-bit hash result

### Using Blowfish Tab
1. Enter an encryption key (4-56 bytes)
2. Enter text to encrypt/decrypt
3. Select Encrypt or Decrypt mode
4. Click "Process"
5. View and copy the result

## Test Results ✅

Both features have been tested and verified working:

### MD5 Test
- **Input:** "Hello, World!"
- **Output:** `65a8e27d8879283831b664bd8b7f0ad4` ✓

### Blowfish Test
- **Key:** "mySecretKey123"
- **Plaintext:** "Secret Message"
- **Encrypted:** `72b1a09a7c38a2ef890c79ddd2eabc60`
- **Decrypted:** "Secret Message" ✓

## Features

### UI/UX
- Clean, modern interface with purple gradient
- Smooth tab switching with animations
- Loading indicators during computation
- Status messages (success/error)
- Copy-to-clipboard for results
- Fully responsive design

### Functionality
- Real-time processing
- Hex encoding for binary data
- Error handling with user-friendly messages
- Key validation (Blowfish)
- Input validation

## Browser Compatibility
- Chrome/Chromium (latest)
- Firefox (latest)
- Safari (latest)
- Edge (latest)
- Any browser with WebAssembly support

## Architecture

```
User Interface (HTML/CSS/JavaScript)
           ↓
    Emscripten Module (WASM)
           ↓
    C++ Cryptographic Functions
    - Blowfish (src/blowfish/)
    - MD5 (src/md5/)
    - RSA-Small (src/rsa-small/)
```

## Notes

- This is an **educational implementation** - NOT suitable for production security
- Blowfish and MD5 are educational implementations and have known vulnerabilities
- The WASM module runs entirely in the browser - no server required
- All processing happens locally in the user's browser

## Build Configuration

The `build_wasm.sh` script uses:
- **emcc** (Emscripten compiler)
- **--bind** flag for C++ embind bindings
- **-O3** optimization level
- **ALLOW_MEMORY_GROWTH** for dynamic memory
- C++17 standard

## Future Enhancements

Potential improvements:
- Add RSA encryption support
- Support for file uploads/downloads
- Additional cipher algorithms
- Batch processing
- Settings/preferences
- Dark mode toggle

---

**Created with Emscripten** - Compiling C++ to WebAssembly for the browser!
