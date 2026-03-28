# Plan implementacji — Etap A (small-int, int/long prototype)

Cel: szybkie i przejrzyste zweryfikowanie logiki RSA używając wyłącznie wbudowanych typów całkowitych (`unsigned long` / `uint32_t` / `uint64_t`) w środowisku edukacyjnym.

Założenia i ograniczenia
- Typy: `uint64_t`, by działać w standardowym C++ bez zewnętrznych bibliotek bignum.
- CSPRNG: `std::random_device` + `std::mt19937_64` jako edukacyjny placeholder.
  - **Uwaga**: MT-19937 nie jest cryptographically secure RNG
  - Dla edukacji: wystarczajace, dla produkcji: /dev/urandom lub systemowy RNG
- Zakres bitów: 16, 24, 32, maksymalnie 48 bitów (ze względu na overflow przy mnożeniu w `uint64_t`).
  - **Uwaga**: 64-bitowy klucz (p,q po 32 bity) nie zmieści się w uint64 bez overflow przy `p*q` i `(p-1)*(q-1)`.
  - Dla kluczy >48 bitów wymagany `__uint128_t` lub BigInt.

Kolejność implementacji (prosty krok-po-kroku)

1) Szkielet projektu
- `src/arithmetics.h`, `src/arithmetics.cpp`, `src/rsa_small.h`, `src/rsa_small.cpp`, `tests/test_arithmetics.cpp`, `tests/test_rsa_small.cpp`.
- Prosty build przez `g++` (task z workspace). 

2) Podstawowy `uint64_t`
- używamy bezpośrednio `uint64_t` i standardowych operatorów (+, -, *).
- Dodatkowe funkcje pomocnicze (add/sub/mul) są opcjonalne, tylko jeśli potrzebna kontrola overflow.
- Funkcje obowiązkowe:
  - ~~`std::pair<uint64_t, uint64_t> divmod(uint64_t a, uint64_t b)`~~ to już jest wbudowane: div()
  - `uint64_t mod_pow(uint64_t base, uint64_t exp, uint64_t mod)`
  - `uint64_t egcd(uint64_t a, uint64_t b, uint64_t &x, uint64_t &y)` 
  - `uint64_t mod_inv(uint64_t a, uint64_t m)`

https://github.com/TheAlgorithms/C-Plus-Plus/tree/master/math

3) Modular exponentiation
- Square-and-multiply.
- Testy: prosta pętla vs `mod_pow`, oraz własność Fermata dla małych pierwszych.

4) Extended Euclid i odwrotność modularna
- `egcd` (iteracyjnie lub rekurencyjnie), `mod_inv` na bazie `egcd`.
- Testy: `a*inv % m == 1` gdy gcd==1 i przypadki braku odwrotności.

5) Prosty test pierwszości (Miller-Rabin + filtr)
- **Bazy testowe dla uint64_t**: {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}
- Lista małych dzielników do 1000 (szybki filtr)
- `is_probable_prime(n,k)` z k=5..10 rundami Miller-Rabin
  - Dla `n <= 37`: użyj trial division (bez MR)
- testy z wektorami znanych liczb pierwszych i złożonych

6) Generowanie liczby pierwszej
- `random_odd(bits)`:
  - Ustawia bit najwyzszy (index `bits-1`) = 1
  - Ustawia bit najmniej znaczacy = 1 (liczba nieparzysta)
  - Pozostale bity losowe
- Kolejność: generuj liczby, filtr małych dzielników, Miller-Rabin.
- **Limit iteracji**: max 100000 prob, jeśli nie znaleziona - exception
- Debug output: czasu, liczby prob, liczby odrzuconych przez filtr

7) Generacja pary kluczy RSA
- `p`, `q` po `bits/2`.
- `n=p*q`, `phi=(p-1)*(q-1)`.
- `e=65537` z fallback na losowe `e`:
  - Losuj nieparzyste `e` z zakresu [3, phi-1]
  - Fallback limit: max 10000 iteracji
  - Walidacja: `gcd(e, phi) == 1`
- `d = mod_inv(e, phi)`.
- **Obowiązkowa walidacja**:
  - `assert(p != q)` - p i q muszą byc rozne
  - `assert((e * d) % phi == 1)`
  - **Test decrypt(encrypt(m)) == m** dla kilka losowych `m < n` (opcjonalnie w debug, obowiązkowo w testach)

8) Krotki typ klucza i serializacja tekstowa
- `struct RSAKeySmall { uint64_t n,e,d,p,q; };`
- `serialize_key` / `deserialize_key` w formacie JSON: `{"n": ..., "e": ..., "d": ..., "p": ..., "q": ...}`

9) Szyfrowanie / odszyfrowanie prostym RSA
- `encrypt_raw(m,e,n)` i `decrypt_raw(c,d,n)` via `mod_pow`.
- **Obowiankowe walidacje**:
  - `m >= n` - reject (exception)
  - `c >= n` - reject (exception)
- Testy integracyjne: dla losowych `m<n` sprawdz `decrypt(encrypt(m))==m`

10) Podpis i weryfikacja (nauka)
- **Haszowanie**: SHA-256 (mozna inline'owac prosta implementacje)
  - **ZAKAZANE**: `std::hash<std::string>` (nie jest kryptograficznie bezpieczna)
- Hash result: modulo n
- `sign(msg,d,n)`, `verify(msg,s,e,n)`
- Testy: poprawny podpis musi verify, zmodyfikowany tekst musi falszywie

11) Testy jednostkowe i integracyjne
- **Modul smallint**: `mod_pow`, `egcd`, `mod_inv`, `divmod`
- **Prime testing**: `is_probable_prime` (znane pierwsze, znane zlozone)
- **Key generation**: `keygen` (p != q, (e*d) % phi == 1)
- **Encryption/Decryption**: `encrypt/decrypt` (roundtrip, m >= n reject)
- **Signature**: `sign/verify` (poprawny, zmodyfikowany tekst falszywy)
- **Testy negatywne**: m>=n (exception), e bez odwrotnosci, uszkodzony podpis

12) Constant-time i side-channel security (edukacyjne uwagi)
- `mod_pow` w encryption/decryption jest podatna na timing attacks
- **To jest znane ograniczenie tego wariantu edukacyjnego**
- Memory cleanup: tajne wartosci (`d`, `p`, `q`) powinne byc zerowane
- **TODO**: Dodac w przyszlego BigInt wariantu

13) Dokumentacja migracji na BigInt
- Lista funkcji do przejscia: `mod_pow`, `egcd`, `mod_inv`, `is_probable_prime`, `keygen`, `encrypt/decrypt`, `sign/verify`
- Miejsca wymagajace constant-time: `mod_pow` (dla private key), `mod_inv`
- TODO: memory cleanup, proper CSPRNG

Uwagi bezpieczestwa
- projekt edukacyjny; nie do produkcyjnego uzytku.
- **ZAKAZANE**: `std::hash`, `rand()` - nie sa kryptograficznie bezpieczne
- **UWAGA**: brak constant-time, brak memory cleanup - edukacyjny wariant tylko
- **UWAGA**: MT19937 nie jest CSPRNG - dla nauki OK, dla produkcji nie

