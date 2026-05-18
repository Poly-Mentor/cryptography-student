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
import hashlib

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


def blowfish_text_roundtrip():
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


def blowfish_file_roundtrip():
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


def blowfish_text_roundtrip_keyfile():
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


def blowfish_file_roundtrip_keyfile():
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


def md5_text():
    """Compute MD5 of a literal string via the CLI and compare to Python's hashlib."""
    print("=== MD5 text ===")
    out = run_cmd([CRYPTOTOOL, "md5", "-t", "hello world"]).strip()
    expected = hashlib.md5(b"hello world").hexdigest()
    if out.strip() != expected:
        print("MD5 text mismatch", file=sys.stderr)
        print("cli:", out)
        print("py :", expected)
        sys.exit(1)
    print("MD5 text OK")


def md5_file():
    """Compute MD5 of a temporary file via the CLI and compare to hashlib."""
    print("=== MD5 file ===")
    f = tempfile.NamedTemporaryFile(delete=False)
    path = f.name
    try:
        f.write(b"hello world")
        f.close()
        out = run_cmd([CRYPTOTOOL, "md5", "-f", path]).strip()
        with open(path, "rb") as fh:
            expected = hashlib.md5(fh.read()).hexdigest()
        if out.strip() != expected:
            print("MD5 file mismatch", file=sys.stderr)
            print("cli:", out)
            print("py :", expected)
            sys.exit(1)
        print("MD5 file OK")
    finally:
        try:
            os.remove(path)
        except Exception:
            pass


def md5_file_output():
    """Run CLI with -f and -o to write hash to an output file and verify contents."""
    print("=== MD5 file output ===")
    in_f = tempfile.NamedTemporaryFile(delete=False)
    in_path = in_f.name
    in_f.write(b"hello world")
    in_f.close()

    out_f = tempfile.NamedTemporaryFile(delete=False)
    out_path = out_f.name
    out_f.close()

    try:
        run_cmd([CRYPTOTOOL, "md5", "-f", in_path, "-o", out_path])
        with open(in_path, "rb") as fh:
            expected = hashlib.md5(fh.read()).hexdigest()
        with open(out_path, "r") as of:
            got = of.read().strip()
        if got != expected:
            print("MD5 file output mismatch", file=sys.stderr)
            print("file:", got)
            print("exp :", expected)
            sys.exit(1)
        print("MD5 file output OK")
    finally:
        for p in (in_path, out_path):
            try:
                os.remove(p)
            except Exception:
                pass

def test_rsa():
    """Test key generation functionality with multiple parameter combinations."""

    print("=== RSA generate/encrypt/decrypt ===")

    # Ensure binary is up-to-date
    run_cmd(["make", "build"])

    # 1) Generate keys into a temporary directory and use key files
    print("Testing RSA key generation and keyfile-based encryption/decryption...")
    out_dir = tempfile.TemporaryDirectory()
    out_path = out_dir.name
    try:
        run_cmd([CRYPTOTOOL, "rsa", "-g", "-l", "16", "-o", out_path])

        pub_path = os.path.join(out_path, "public_key.txt")
        priv_path = os.path.join(out_path, "private_key.txt")

        if not os.path.isfile(pub_path) or not os.path.isfile(priv_path):
            print("Key files missing after generation", file=sys.stderr)
            sys.exit(1)

        with open(pub_path, "r") as pf:
            pub = pf.read().strip()
        with open(priv_path, "r") as sf:
            priv = sf.read().strip()

        # 2) Encrypt/decrypt a small text using key strings (-p / -s)
        print("Testing RSA encryption/decryption with key strings...")
        enc = run_cmd([CRYPTOTOOL, "rsa", "-e", "-t", "hello", "-p", pub]).strip()
        if not re.fullmatch(r"([0-9a-fA-F]+( [0-9a-fA-F]+)*)", enc):
            print("Encryption output not hex blocks:", file=sys.stderr)
            print(enc, file=sys.stderr)
            sys.exit(1)
        dec = run_cmd([CRYPTOTOOL, "rsa", "-d", "-t", enc, "-s", priv])
        if "hello" not in dec:
            print("Decryption failed or unexpected output:", file=sys.stderr)
            print(dec, file=sys.stderr)
            sys.exit(1)

        # 3) Generate keys to stdout and parse key strings
        print("Testing RSA key generation to stdout and parsing keys...")
        stdout_gen = run_cmd([CRYPTOTOOL, "rsa", "-g", "-l", "16"])
        m_pub = re.search(r"Public Key:\s*(\S+)", stdout_gen)
        m_priv = re.search(r"Private Key:\s*(\S+)", stdout_gen)
        if not m_pub or not m_priv:
            print("Failed to parse keys from stdout:", file=sys.stderr)
            print(stdout_gen, file=sys.stderr)
            sys.exit(1)
        pub2 = m_pub.group(1)
        priv2 = m_priv.group(1)

        # Verify encryption/decryption using keys printed to stdout
        enc2 = run_cmd([CRYPTOTOOL, "rsa", "-e", "-t", "hi", "-p", pub2]).strip()
        if not re.fullmatch(r"([0-9a-fA-F]+( [0-9a-fA-F]+)*)", enc2):
            print("Encryption output not hex blocks (stdout keys):", file=sys.stderr)
            print(enc2, file=sys.stderr)
            sys.exit(1)
        dec2 = run_cmd([CRYPTOTOOL, "rsa", "-d", "-t", enc2, "-s", priv2])
        if "hi" not in dec2:
            print("Decryption (stdout keys) failed:", file=sys.stderr)
            print(dec2, file=sys.stderr)
            sys.exit(1)

        # 4) File encryption/decryption using key files (-P / -S)
        print("Testing RSA file encryption/decryption with key files...")
        in_f = tempfile.NamedTemporaryFile(delete=False)
        in_path = in_f.name
        in_f.write(b"hello file")
        in_f.close()

        enc_f = tempfile.NamedTemporaryFile(delete=False)
        enc_path = enc_f.name
        enc_f.close()

        dec_f = tempfile.NamedTemporaryFile(delete=False)
        dec_path = dec_f.name
        dec_f.close()

        try:
            run_cmd([CRYPTOTOOL, "rsa", "-e", "-f", in_path, "-P", pub_path, "-o", enc_path])
            run_cmd([CRYPTOTOOL, "rsa", "-d", "-f", enc_path, "-S", priv_path, "-o", dec_path])

            with open(in_path, "rb") as a, open(dec_path, "rb") as b:
                if a.read() != b.read():
                    print("File roundtrip mismatch", file=sys.stderr)
                    sys.exit(1)
        finally:
            for p in (in_path, enc_path, dec_path):
                try:
                    os.remove(p)
                except Exception:
                    pass

        # 5) File encryption with key string (-p) and decryption with key string (-s)
        in_f2 = tempfile.NamedTemporaryFile(delete=False)
        in_path2 = in_f2.name
        in_f2.write(b"hello file 2")
        in_f2.close()

        enc_f2 = tempfile.NamedTemporaryFile(delete=False)
        enc_path2 = enc_f2.name
        enc_f2.close()

        dec_f2 = tempfile.NamedTemporaryFile(delete=False)
        dec_path2 = dec_f2.name
        dec_f2.close()

        try:
            run_cmd([CRYPTOTOOL, "rsa", "-e", "-f", in_path2, "-p", pub, "-o", enc_path2])
            run_cmd([CRYPTOTOOL, "rsa", "-d", "-f", enc_path2, "-s", priv, "-o", dec_path2])
            with open(in_path2, "rb") as a, open(dec_path2, "rb") as b:
                if a.read() != b.read():
                    print("File roundtrip (string keys) mismatch", file=sys.stderr)
                    sys.exit(1)
        finally:
            for p in (in_path2, enc_path2, dec_path2):
                try:
                    os.remove(p)
                except Exception:
                    pass

        # 6) Negative tests: missing keys should return error
        try:
            run_cmd([CRYPTOTOOL, "rsa", "-e", "-t", "no-key"])
            print("Expected failure when no public key provided", file=sys.stderr)
            sys.exit(1)
        except RuntimeError as e:
            if "You must provide a public key" not in str(e):
                print("Unexpected error for missing public key:", e, file=sys.stderr)
                sys.exit(1)

        try:
            run_cmd([CRYPTOTOOL, "rsa", "-d", "-t", "no-key"])
            print("Expected failure when no private key provided", file=sys.stderr)
            sys.exit(1)
        except RuntimeError as e:
            if "You must provide a private key" not in str(e):
                print("Unexpected error for missing private key:", e, file=sys.stderr)
                sys.exit(1)

        print("RSA CLI OK")
    finally:
        out_dir.cleanup()

def main():
    if not ensure_executable(CRYPTOTOOL):
        return 2

    blowfish_text_roundtrip()
    blowfish_file_roundtrip()
    blowfish_text_roundtrip_keyfile()
    blowfish_file_roundtrip_keyfile()

    md5_text()
    md5_file()
    md5_file_output()

    test_rsa()

    print("All tests passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
