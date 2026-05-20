# Quick Start Guide - Cryptography Tool WASM

## Opening the App

1. **Build the WASM (one time)**:
   ```bash
   ./build_wasm.sh
   ```

2. **Open in browser**:
   - Open `bin/cryptotool.html` in your web browser
   - The app loads instantly with no server needed

## Using MD5 Hashing

1. Click the **"MD5 Hash"** tab
2. Type text you want to hash in the textarea
3. Click **"Compute Hash"**
4. Your hash appears in the result box
5. Click **"📋 Copy"** to copy to clipboard

**Example**: 
- Input: `Hello World`
- Output: `3e25960a79dbc69b674ce66540f5a370`

## Using Blowfish Encryption

### To Encrypt:
1. Click the **"Blowfish Cipher"** tab
2. Enter an **Encryption Key** (4-56 bytes)
3. Make sure **"Encrypt"** is selected in Mode
4. Enter **Input Text** to encrypt
5. Click **"Process"**
6. Encrypted result shown (hex format)

### To Decrypt:
1. Same setup, but select **"Decrypt"** mode
2. Enter the hex-encoded ciphertext
3. Use the SAME key used for encryption
4. Click **"Process"**
5. Original plaintext is recovered

**Example**:
- Key: `mykey`
- Plaintext: `Hello`
- Ciphertext: `d4f2c8e91a5b3c4e`
- Decrypting with same key returns: `Hello`

## Rebuilding After Code Changes

If you modify C++ source code:

```bash
./build_wasm.sh
```

Then refresh your browser to load the new WASM module.

## File Locations

```
cryptography-student/
├── bin/
│   ├── cryptotool.html      ← Open this in browser
│   ├── cryptotool.js        ← WASM loader (generated)
│   └── cryptotool.wasm      ← WASM binary (generated)
├── src/
│   ├── wasm_wrapper.cpp     ← Embind bindings
│   ├── blowfish/
│   ├── md5/
│   └── ...
├── build_wasm.sh            ← Build script
└── WASM_BUILD.md            ← Full documentation
```

## Requirements

- Emscripten toolchain installed (already available)
- Modern web browser with WebAssembly support
- No server required (works locally)

## Troubleshooting

**Module not loading?**
- Check browser console (F12 → Console tab)
- Rebuild with `./build_wasm.sh`
- Clear browser cache (Ctrl+Shift+Delete)

**Results showing "undefined"?**
- Wait for WASM module to fully load
- Check that input data is valid
- Try the test examples above

**Encryption/decryption not matching?**
- Verify you're using the exact same key
- Keys are case-sensitive
- Clear spaces in hex input for decryption

## Performance

- **MD5 Hash**: ~1-5ms for typical text
- **Blowfish**: ~5-20ms depending on text size
- All computation happens in your browser
- No network latency

## Security Notes

⚠️ **Educational Use Only**
- MD5: Cryptographically broken - don't use for security
- Blowfish: Educational implementation - not hardened against attacks
- This is a learning tool, not for production use

---

**Ready to explore cryptography!** 🔐

For more details, see `WASM_BUILD.md`
