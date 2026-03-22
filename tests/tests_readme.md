# Testy jednostkowe Blowfish

Krótka instrukcja budowy i uruchomienia testów jednostkowych dodanych w `tests/test_blowfish.cpp`.

Wymagania
- `g++` (kompilator C++ z obsługą C++17)
- katalog roboczy zawierający pliki `src/blowfish.cpp`, `src/blowfish.h`, `src/crypto_bf_bf_pi.h` i `tests/test_blowfish.cpp`.

Kompilacja
Utwórz katalog `bin` jeśli go nie ma, a następnie skompiluj (nowa struktura projektu używa `src/` i `tests/`):

```bash
mkdir -p bin
g++ -std=c++17 -O2 src/blowfish.cpp tests/test_blowfish.cpp -I. -o bin/test_blowfish
```

Uruchomienie testu

```bash
./bin/test_blowfish
```

Oczekiwany wynik
- Testy wypiszą kilka linii `PASS:` dla poszczególnych przypadków, a na końcu `ALL TESTS PASSED`.

Uwagi
- Testy używają trików deweloperskich (macro `#define private public`) by mieć dostęp do prywatnych metod; to jest tylko do testów lokalnych.
- Jeśli kompilator zgłasza braki symboli, upewnij się, że `crypto_bf_bf_pi.h` (z inicjalizacją `bf_init`) jest dostępny i poprawnie dołączony.
- Możemy później przerobić testy na framework (Catch2/GoogleTest) jeśli chcesz.
