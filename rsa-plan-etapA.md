# Plan implementacji — Etap A (small-int, int/long prototype)

Cel: szybkie, krokowe zweryfikowanie logiki RSA przy użyciu natywnych typów całkowitych (int/long). Nie do użytku produkcyjnego — tylko walidacja algorytmów przed migracją do bignum.

Założenia i ograniczenia
- Typy: używamy `uint32_t` / `uint64_t` (albo `unsigned long`) w zależności od potrzeb bitowych.
- CSPRNG: używamy systemowego źródła losowości (`std::random_device` / `/dev/urandom`) do generowania kandydatów.
- Zakres bitów: testy na małych rozmiarach (np. 16, 32, 48 bitów) — tak by operacje mieściły się w typach.

Kolejność implementacji (szczegółowy plan krok-po-kroku)

1) Szkielet projektu i konwencje
- Utwórz pliki/szkielety: `src/smallint.h`, `src/smallint.cpp`, `src/rsa_small.h`, `src/rsa_small.cpp`, `tests/test_rsa_small.cpp`.
- Przygotuj prosty build (użyj istniejącego `g++` task albo prostego `Makefile`).

2) `SmallIntAdapter` (interfejs dla prototypu)
- Definicja: typ alias `using SmallInt = uint64_t;` (albo `uint32_t` dla mniejszych testów).
- Funkcje eksportowane:
  - `SmallInt add(SmallInt a, SmallInt b)` — opcjonalnie z detekcją overflow dla testów.
  - `SmallInt sub(SmallInt a, SmallInt b)`
  - `SmallInt mul(SmallInt a, SmallInt b)`
  - `std::pair<SmallInt, SmallInt> divmod(SmallInt a, SmallInt b)`
  - `SmallInt mod_pow(SmallInt base, SmallInt exp, SmallInt mod)`
  - `SmallInt egcd(SmallInt a, SmallInt b, SmallInt &x, SmallInt &y)` (rozszerzony Euklides)
  - `SmallInt mod_inv(SmallInt a, SmallInt m)`

3) Implementacja modular exponentiation (efektywne potęgowanie modularne)
- Implementuj `mod_pow` używając square-and-multiply (binary exponentiation).
- Zaimplementuj testy jednostkowe: porównanie z prostą pętlą (dla małych wykładników) oraz testy własności (a^{phi} ≡ 1 mod p dla p pierwszego, jeśli dotyczy).

4) Extended Euclidean Algorithm i modular inverse
- Implementuj `egcd` (iteracyjnie lub rekurencyjnie) i `mod_inv(a,m)` wykorzystując `egcd`.
- Testy: sprawdź, że `(a * mod_inv(a,m)) % m == 1` gdy gcd==1; testy przeciwne, gdy odwrotność nie istnieje.

5) Test pierwszości (Miller–Rabin) i prosta filtracja
- Implementuj listę małych pierwszych (np. do 1000) do szybkiej filtracji kandydatów.
- Implementuj `is_probable_prime(n, k)` (Miller–Rabin) z parametrem rund `k`.
- Dla 32/64-bitów możesz użyć deterministycznych baz znanych dla tego zakresu (opcjonalnie) — dla Etapu A wystarczą małe, testowe podstawy.
- Testy: wektory znanych liczb pierwszych i złożonych.

6) Generowanie kandydatów na liczby pierwsze
- Funkcja: `SmallInt random_odd(bits)` — generuje liczbę o podanej długości bitowej, ustawia najwyższy bit i bit parzystości (odd).
- Stosuj filtr małych pierwszych, potem Miller–Rabin.
- Zapewnij limit prób i właściwe logowanie (dla testów określ maks. iteracji).

7) Generacja klucza RSA (małe p, q)
- Procedura:
  - Wybierz rozmiar p i q (np. po połowie wymaganego rozmiaru n).
  - Generuj p, q przy użyciu kroku 6; upewnij się, że `p != q` i że `abs(p-q)` nie jest zbyt małe (dla bezpieczeństwa edukacyjnego).
  - Oblicz `n = p * q` (upewnij się, że mieści się w `SmallInt`).
  - Oblicz `phi = (p-1)*(q-1)`.
  - Wybierz `e` (domyślnie 65537) i sprawdź `gcd(e,phi) == 1`; jeśli nie, wybierz inne e lub regeneruj.
  - Oblicz `d = mod_inv(e, phi)`.
- Testy: sprawdź, że `(e * d) % phi == 1` oraz `p` i `q` rzeczywiście pierwsze.

8) Struktura kluczy i serializacja prostego formatu
- Zdefiniuj `struct RSAKeySmall { SmallInt n,e,d; SmallInt p,q; };`
- Funkcje pomocnicze: `serialize_key()` i `deserialize_key()` w prostym tekstowym formacie dla debugowania.

9) Szyfrowanie i odszyfrowanie (bez paddingu, potem z prostym paddingiem)
- Implementuj podstawowe:
  - `SmallInt encrypt_raw(SmallInt m, SmallInt e, SmallInt n) = mod_pow(m, e, n)`
  - `SmallInt decrypt_raw(SmallInt c, SmallInt d, SmallInt n) = mod_pow(c, d, n)`
- Test integracyjny: dla losowych `m < n`, sprawdź `decrypt(encrypt(m)) == m`.
- Następnie dołącz prosty padding-prototyp:
  - Prosty scheme: przed konwersją do liczby dołącz stały nagłówek i losowe bajty (nieprodukcyjne). Alternatywnie zaimplementuj uproszczone OAEP konceptualnie (MGF1 z hashem na bajtach) — niżej minimalne sugestie.

10) Prosty prototyp OAEP / PKCS#1 v1.5 (funkcjonalny, niebezpieczny)
- Dla celu edukacyjnego zaimplementuj uproszczony OAEP:
  - `hash = SHA-256(message)` (użyj biblioteki std/openssl jeśli dostępna; dla Etapu A można użyć `std::hash` jako placeholder).
  - MGF: prosta mask-generation oparta na hashach.
  - Funkcja `oaep_encode(message, k)` i `oaep_decode(encoded, k)` bez zabezpieczeń CT.
- Testy: pozytywne i negatywne (uszkodzony padding powinien być odrzucony).

11) Podpisywanie i weryfikacja (edukacyjne)
- Podpis: `s = mod_pow(hash(message), d, n)` (hash jako liczba zmniejszona modulo n).
- Weryfikacja: `mod_pow(s, e, n) == hash(message)`.
- Testy: sygnatura poprawna dla różnych komunikatów; niepoprawna przy zmianie komunikatu.

12) Testy jednostkowe i integracyjne
- Jednostkowe (funkcje): `mod_pow`, `egcd`, `mod_inv`, `is_probable_prime`.
- Integracyjne: cały flow: keygen -> encrypt -> decrypt; keygen -> sign -> verify.
- Negatywne testy: nieprawidłowe paddingi, `m >= n`, `e` nieodwracalny.

13) Logging, diagnostyka i limity testów
- Dodaj logowanie/assercje w trybie debug: liczba iteracji próby znalezienia pierwszej, porażki M-R, komunikaty o overflow.

14) Dokumentacja i checklista migracji na BigInt
- Po zakończeniu Etapu A przygotuj krótki raport zawierający:
  - Mapowanie funkcji `SmallInt` → interfejs `BigIntLike` (nazwy funkcji, sygnatury),
  - Lista testów jednostkowych, które muszą zostać przeniesione/uruchomione ponownie z backendem bignum,
  - Potencjalne miejsca wymagające constant-time i blindingu w backendzie bignum.

Pliki sugerowane do utworzenia (minimalne)
- `src/smallint.h`, `src/smallint.cpp` — adapter + mod_pow + egcd + mod_inv
- `src/rsa_small.h`, `src/rsa_small.cpp` — keygen, encrypt/decrypt, sign/verify
- `src/oaep_small.h`, `src/oaep_small.cpp` — prosty padding prototyp
- `tests/test_smallint.cpp`, `tests/test_rsa_small.cpp` — jednostkowe + integracyjne

Kryteria ukończenia Etapu A
- Wszystkie jednostkowe funkcje przechodzą testy (mod_pow, egcd, mod_inv, is_prime).
- Generacja pary kluczy działa powtarzalnie dla zadanych parametrów i przechodzi podstawowe warunki.
- Dla losowych `m < n`, `decrypt(encrypt(m)) == m`.
- Podpis-weryfikacja działa poprawnie dla testowych komunikatów.
- Przygotowany dokument migracji do `BigIntLike`.

Uwagi bezpieczeństwa (przypomnienie)
- Etap A to prototyp edukacyjny — nie używać do rzeczywistych danych. Zamiast `std::hash` użyć rzeczywistego algorytmu hash (np. SHA-256) po migracji.
- Upewnij się, że w migracji do bignum zadbasz o CSPRNG, padding OAEP i constant-time.

Jeśli chcesz, mogę teraz:
- wygenerować szkielet plików źródłowych (nagłówki i puste implementacje) oraz testów, albo
- stworzyć konkretne implementacje funkcji `mod_pow`, `egcd` i `mod_inv` w C++ dla prototypu.
