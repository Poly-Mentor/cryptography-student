#!/usr/bin/env python3
"""
Python test script for `./bin/cryptotool`.
This script mirrors the bash test and uses `subprocess.run`.
"""

import subprocess
import tempfile
import os
import re
import sys

# Path to the CLI binary we will test
CRYPTOTOOL = "./bin/cryptotool"


def run_cmd(args):
    """Run a command and return stdout (text). Raise RuntimeError on non-zero exit.

    Args:
        args (list): command and arguments

    Returns:
        str: stdout from the command
    """
    proc = subprocess.run(args, capture_output=True, text=True)
    if proc.returncode != 0:
        raise RuntimeError(
            f"Command failed (code {proc.returncode}): {' '.join(args)}\n"
            f"stderr: {proc.stderr.strip()}"
        )
    return proc.stdout


def ensure_executable(path):
    if not os.path.isfile(path) or not os.access(path, os.X_OK):
        print(f"Error: {path} not found or not executable", file=sys.stderr)
        return False
    return True


def text_roundtrip():
    print("=== Blowfish text roundtrip ===")
    # Encrypt a literal string
    enc = run_cmd([CRYPTOTOOL, "blowfish", "-e", "-t", "hello world", "-k", "password"]).strip()
    enc = enc.strip()

    # Ensure the output looks like hex
    if not re.fullmatch(r"[0-9a-fA-F]+", enc):
        print(f"Encryption output not hex: '{enc}'", file=sys.stderr)
        sys.exit(1)

    # Decrypt and check the original text is present in output
    dec = run_cmd([CRYPTOTOOL, "blowfish", "-d", "-t", enc, "-k", "password"])  # may contain label text
    if "hello world" not in dec:
        print("Decryption failed or unexpected output:")
        print(dec)
        sys.exit(1)

    print("Text roundtrip OK")


def file_roundtrip():
    print("=== Blowfish file roundtrip ===")

    # Create temporary files. delete=False so we can pass filenames to subprocess.
    in_f = tempfile.NamedTemporaryFile(delete=False)
    in_path = in_f.name
    in_f.write(b"hello world")
    in_f.close()

    enc_f = tempfile.NamedTemporaryFile(delete=False)
    enc_path = enc_f.name
    enc_f.close()

    dec_f = tempfile.NamedTemporaryFile(delete=False)
    dec_path = dec_f.name
    dec_f.close()

    try:
        # Encrypt input file to enc_path
        run_cmd([CRYPTOTOOL, "blowfish", "-e", "-f", in_path, "-k", "password", "-o", enc_path])

        # Decrypt encrypted file to dec_path
        run_cmd([CRYPTOTOOL, "blowfish", "-d", "-f", enc_path, "-k", "password", "-o", dec_path])

        # Compare the original and decrypted files
        with open(in_path, "rb") as a, open(dec_path, "rb") as b:
            a_data = a.read()
            b_data = b.read()
            if a_data != b_data:
                print("File roundtrip mismatch", file=sys.stderr)
                print("Original:")
                print(a_data.decode(errors="replace"))
                print("Decrypted:")
                print(b_data.decode(errors="replace"))
                sys.exit(1)

        print("File roundtrip OK")

    finally:
        # Clean up temporary files
        for p in (in_path, enc_path, dec_path):
            try:
                os.remove(p)
            except Exception:
                pass


def text_roundtrip_keyfile():
    """Encrypt/decrypt using a key supplied via a temporary key file (-K)."""
    print("=== Blowfish text roundtrip (key file) ===")
    # Create a temporary key file containing the password text
    kf = tempfile.NamedTemporaryFile(delete=False)
    key_path = kf.name
    try:
        kf.write(b"password")
        kf.close()

        enc = run_cmd([CRYPTOTOOL, "blowfish", "-e", "-t", "hello world", "-K", key_path]).strip()
        enc = enc.strip()

        if not re.fullmatch(r"[0-9a-fA-F]+", enc):
            print(f"Encryption output not hex: '{enc}'", file=sys.stderr)
            sys.exit(1)

        dec = run_cmd([CRYPTOTOOL, "blowfish", "-d", "-t", enc, "-K", key_path])
        if "hello world" not in dec:
            print("Decryption with key file failed or unexpected output:")
            print(dec)
            sys.exit(1)

        print("Text roundtrip with key file OK")
    finally:
        try:
            os.remove(key_path)
        except Exception:
            pass


def file_roundtrip_keyfile():
    """Encrypt/decrypt files using a key supplied via a temporary key file (-K)."""
    print("=== Blowfish file roundtrip (key file) ===")
    # Prepare temp files: input, encrypted, decrypted, and key file
    in_f = tempfile.NamedTemporaryFile(delete=False)
    in_path = in_f.name
    in_f.write(b"hello world")
    in_f.close()

    enc_f = tempfile.NamedTemporaryFile(delete=False)
    enc_path = enc_f.name
    enc_f.close()

    dec_f = tempfile.NamedTemporaryFile(delete=False)
    dec_path = dec_f.name
    dec_f.close()

    kf = tempfile.NamedTemporaryFile(delete=False)
    key_path = kf.name
    try:
        kf.write(b"password")
        kf.close()

        run_cmd([CRYPTOTOOL, "blowfish", "-e", "-f", in_path, "-K", key_path, "-o", enc_path])
        run_cmd([CRYPTOTOOL, "blowfish", "-d", "-f", enc_path, "-K", key_path, "-o", dec_path])

        with open(in_path, "rb") as a, open(dec_path, "rb") as b:
            if a.read() != b.read():
                print("File roundtrip (key file) mismatch", file=sys.stderr)
                sys.exit(1)

        print("File roundtrip with key file OK")
    finally:
        for p in (in_path, enc_path, dec_path, key_path):
            try:
                os.remove(p)
            except Exception:
                pass


def main():
    if not ensure_executable(CRYPTOTOOL):
        return 2

    text_roundtrip()
    file_roundtrip()
    # Additional tests using a key file instead of inline -k
    text_roundtrip_keyfile()
    file_roundtrip_keyfile()

    print("All tests passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
