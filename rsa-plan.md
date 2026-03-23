# Plan implementacji RSA — prosty prototyp (wbudowane typy całkowite)

Cel: prosty wariant RSA do nauki na poziomie `unsigned long`/`uint64_t` bez złożonej abstrakcji BigInt.

1. Założenia
- Typ podstawowy: `uint64_t`.
- Wszystko działa na maszynowych liczbach całkowitych; zakres bitów dobierany w testach (16/24/32/48/64).

2. Moduł `arithmetics` (operacje arytmetyczne)
- `add`, `sub`, `mul`, `divmod` (natywne, z opcjonalną obsługą overflow debug),
- `mod_pow` (square-and-multiply),
- `egcd`, `mod_inv`.

3. Testy podstawowe modułu `arithmetics`
- `mod_pow` vs pętla, własność Fermata dla małych pierwszych
- `egcd` (gcd + współczynniki)
- `mod_inv` (prawidłowa odwrotność i brak gdy gcd!=1)

4. Test pierwszości i generator pierwszych
- Lista małych dzielników i szybki filtr.
- `is_probable_prime(n,k)` Miller–Rabin.
- `random_odd(bits)` + `is_probable_prime` by znaleźć pierwsze wartości.

5. Klasyczne RSA
- `RSAKeySmall { uint64_t n,e,d,p,q; }`.
- `generate_key(bits)` -> p,q,n,phi,e,d (e=65537 lub inne).
- **Walidacja klucza** (OBOWIĄZKOWE):
  - `assert(p != q)` — p i q muszą być różne
  - `assert(gcd(e, phi) == 1)` — e musi być względnie pierwsze z phi
  - `assert((e * d) % phi == 1)` — sprawdzenie matematyczne
- Fallback na losowe `e`: limit max 10000 iteracji, exception jeśli nie znalezione
- `encrypt_raw`, `decrypt_raw` przez `mod_pow`.
- **Test warunku**: `decrypt(encrypt(m)) == m` dla kilka losowych `m < n` (dodać już w keygenku)

6. Proste podpisy
- `hash_to_int(message)` — SHA-256 (można inline'ować prostą implementację) zamiast `std::hash`, `sign(msg,d,n)`, `verify(msg,s,e,n)`.
- **Uwaga**: `std::hash<std::string>` nie jest kryptograficznie bezpieczna (zmienia się między uruchomieniami, ASLR) — **ZAKAZANE** nawet w edukacji.

7. Testy integracyjne
- Klucz -> szyfrowanie/dekryptowanie w pętli losowych m<n.
- Klucz -> podpis/weryfikacja (poprawny i zmodyfikowany tekst).
- Przypadki brzegowe i błędy:
  - `m >= n` powinna być odrzucona (exception)
  - `e` bez odwrotności (gcd(e,phi) != 1) — powinna być obsługiwana w keygenku
  - Zmodyfikowany podpis — verify musi zwrócić false
  - Zły padding (jeżeli zaimplementowany) — decrypt powinno to obsługiwać

8. Serializacja klucza
- Format: JSON (`{"n": ..., "e": ..., "d": ..., "p": ..., "q": ...}`) zamiast `n:e:d:p:q`
- Jest to bardziej przejrzyste i extensible
- Można dodać opcjonalnie Base64 encoding dla portability

9. Opcjonalne dopełnienie paddingiem edukacyjnym
- Prosty schemat gruby "prefix + losowe bajty" dla zasymulowania PKCS#1.
- Bez gwarancji bezpieczeństwa.

10. Obsługa debug i limity
- raport z prób generacji pierwszych,
- debug `MillerRabin` (które bazy odrzucają),
- asercje bezpieczeństwa.
- Overflow testing: eksplicitnie sprawdź, że `p,q < 2^32` jeśli używasz uint64 (aby uniknąć overflow `p*q`)

11. Constant-time i side-channel security (edukacyjne uwagi)
- `mod_pow` użyta w encryption/decryption jest podatna na timing attacks — **to jest znane ograniczenie**
- W przyszłego BigInt-owego: użyj blinding lub constant-time implementation
- Ten wariant jest EDUKACYJNY bez constant-time (odpowiedź na to w dokumentacji migracji)

12. Dokument migracji na BigInt
- lista funkcji `mod_pow`, `egcd`, `mod_inv`, `is_probable_prime`, `keygen`, `encrypt/decrypt`, `sign/verify` do migracji.
- Miejsca wymagające constant-time: `mod_pow` (dla private key operations) i `mod_inv`
- Miejsca wymagające memory cleanup: sekrety w `d`, `p`, `q`

Uwagi
- Edukacyjne: nie do kryptograficznego użytku produkcyjnego.
- Wróć do bignum tylko po skompilowaniu i przetestowaniu prototypu bazowego.

