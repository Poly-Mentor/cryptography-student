**Sprawozdanie z implementacji algorytmów kryptograficznych**

Autor: Filip Sosnowicz


**1. Blowfish**

Implementacja oparta na specyfikacji Bruce'a Schneiera. Realizuje szyfrowanie i deszyfrowanie 64-bitowych bloków (jako 32-bitowe połówki) i 16 rund Feistela przy użyciu wybranego klucza (4–56 bajtów).

- **Struktury danych**: Blok reprezentowany jako `std::pair<uint32_t,uint32_t>` (`Block`). Wewnątrz klasy przechowywane są tablice `uint32_t P[18]` oraz `uint32_t S[4][256]`.
- **Inicjalizacja P i S**: Początkowe wartości P i S pobrane z pliku `crypto_bf_bf_pi.h` (wartości wyprowadzone z cyfr π — nagłówek pochodzi z biblioteki OpenSSL). Następnie wykonywana jest inicjalizacja kluczy (***key schedule***): bajty klucza są powtarzane i XOR-owane z elementami `P`, a potem szyfruje się blok zerowy wielokrotnie, aby nadpisać kolejne elementy `P` i `S` (funkcja `initialize`).
- **Funkcja F**: Implementowana zgodnie z klasycznym opisem: wejście 32-bitowe dzielone na cztery bajty, indeksujące S-boksy, a wynik łączony instrukcjami dodawania/xorowania zgodnie z kodem (`S[0][x0] + S[1][x1] ^ S[2][x2] + S[3][x3]`).
- **Porcjowanie danych**: Zamiana bajtów na bloki używa porządku big-endian — pierwsze 4 bajty tworzą lewą połowę, kolejne 4 bajty prawą (`bytesToBlocks`, `blocksToBytes`).
- **Padding**: PKCS#7 z rozmiarem bloku 8 bajtów (implementacja w `pkcs7Pad` / `pkcs7Unpad`).
- **Interfejs**: Dwa zestawy API: binarne (`encrypt`/`decrypt` przyjmujące `std::vector<uint8_t>`) oraz tekstowe (`encryptText`/`decryptText` pracujące ze `std::string`).
- **Walidacja i wyjątki**: Konstruktor wymaga długości klucza 4–56 bajtów, w przeciwnym razie rzuca wyjątek. Deszyfrowanie sprawdza, czy dane wsadowe są wielokrotnością 8 bajtów.


**2. MD5**

Implementacja zgodna z RFC 1321: przetwarzanie bloków 512-bit (64 bajty), wynikiem jest 128-bitowy hash (16 bajtów).

- **Przygotowanie danych**: `inputToBlocks` dopisuje 0x80, dopełnia zerami do 56 bajtów w ostatnim bloku, a następnie dokleja 64-bitową długość wiadomości w bitach w porządku little-endian.
- **Reprezentacja**: Każdy 512-bitowy blok dzielony jest na 16 słów 32-bitowych w porządku little-endian (zgodnie ze specyfikacją).
- **Główna pętla**: Stałe `k[64]` i `r[64]` są zdefiniowane w nagłówku; `ingestBlock` wykonuje 64 iteracje z odpowiednimi funkcjami nieliniowymi i rotacjami. Rotacje wykonuje `rotate`.
- **Wyjście**: Finalny stan (`A,B,C,D`) jest zapisany w porządku little-endian jako 16 bajtów, a następnie konwertowany na małe litery zapisu heksadecymalnego (`calculate`).
- **Cechy projektowe**: Klasa `MD5` jest nie jest klasą statyczną, ponieważ wykorzystuje atrybuty przechowujące stan podczas wewnętrznego przetwarzania danych, ale jest bezstanowa między wywołaniami publicznej metody `calculate()`, ponieważ stan jest wewnętrznie resetowany przez prywatną metodę `reset()`.


**3. RSA**

- **Ograniczenia**: Dla uproszczenia wykorzystano operacje na `uint64_t`. Implementacja wspierająca realnie stosowane rozmiary kluczy (1024-4096 bitów), wymagałaby użycia bibliotek do działania na liczbach wielkiej precyzji, co wykracza poza zakres tego projektu.
- **Generowanie kluczy** (`RSA::generate_keys`): dzieli żądaną długość bitową na rozmiary `p` i `q`, generuje liczby nieparzyste o zadanej długości bitów przy pomocy `RNG::get_random_odd`, testuje je funkcją `Arithmetics::is_probable_prime` (algorytm Millera–Rabina z predefiniowanym zestawem podstaw, które zapewniają pewną pierwszość w całym 64-bitowym zakresie), zapewnia `p != q`, oblicza `n = p * q`, `phi = (p-1)*(q-1)`, wybiera `e` losowo tak, by `gcd(e,phi) == 1`, i oblicza `d = mod_inv(e, phi)`.
- **Arithmetics**: `mod_pow` używa algorytmu szybkiego potęgowania; `mul_mod` korzysta z `unsigned __int128` jako pośrednika, by uniknąć przepełnienia przy mnożeniach 64-bitowych; `egcd` to implementacja rozszerzonego algorytmu Euklidesa, a `mod_inv` wykorzystuje `egcd` do znalezienia odwrotności modularnej.
- **Szyfrowanie / odszyfrowanie**: Każdy bajt tekstu jest traktowany jako liczba 0–255 i szyfrowany osobno: `c = m^e mod n`. Odszyfrowanie wykonuje `m = c^d mod n` i weryfikuje, że wynik mieści się w bajcie (w przeciwnym przypadku rzuca wyjątek).
- **Interfejs klucza**: `keyToString` / `stringToKey` serializują/parsują klucz w formacie `exponent,modulus`.
- **Generator losowy**: `RNG` używa `std::rand()` z nasieniem czasu (`std::srand(time(nullptr))`) — celowo prosty wybór dla demonstracji, nieodpowiedni do zastosowań kryptograficznych.

**Decyzje projektowe i uzasadnienia**
- **Czytelność i walory edukacyjne**: Głównym celem była przejrzystość implementacji — kod ma ilustrować mechanikę algorytmów bez aplikowania złożonych optymalizacji ani używania zewnętrznych bibliotek big-int.
- **Obsługa błędów**: wejścia walidowane są poprzez wyjątki (`std::invalid_argument`, `std::runtime_error`) aby ułatwić wykrycie nieprawidłowych danych.
- **Modułowość**: Każdy algorytm ma własny moduł/klasę, oddzielone pliki nagłówkowe i implementacyjne, co poprawia czytelność i ułatwia testowanie.
- **Testowalność**: W celu zapewnienia, że poszczególne elementy kodu działają poprawnie, zaimplementowano testy jednostkowe i integracyjne (cały cykl szyfrowanie-deszyfrowanie, lub porównanie wyników MD5 z testowymi wektorami wygenerowanymi zewnętrznie).


**4. Kod pomocniczy**

- **File**: Klasa `File` (`src/utils/utils.h` + `src/utils/utils.cpp`) — prosty wrapper operacji plikowych:
  - **Konstruktorzy**:
    - `File()` — konstruktor domyślny (pusty obiekt).
    - `File(std::filesystem::path path)` — jeśli plik istnieje, wczytanie treści odbywa się ***leniwie***; jeśli plik nie istnieje, tworzy pusty plik; weryfikuje czy ścieżka jest zwykłym plikiem.
    - `File(std::filesystem::path path, std::vector<uint8_t> contentBytes)` — zapis binarny zawartości i cache treści.
    - `File(std::filesystem::path path, std::string content)` — zapis tekstu w trybie binarnym i cache.
  - **Metody**:
    - `const std::vector<uint8_t>& getContentBytes()` — lazy-load, zwraca referencję do wektora bajtów (cache).
    - `File* saveFileAs(const std::vector<uint8_t>& contentBytes, std::filesystem::path targetPath)` — zapis do nowej ścieżki; zwraca wskaźnik do nowo utworzonego obiektu `File`.
  - **Implementacja i uwagi**:
    - Leniwe wczytywanie realizowane jest przez statyczną `readFileToBytes`, która używa `std::ifstream` w trybie binarnym i `std::istreambuf_iterator` do wypełnienia wektora bajtów (alokowanego dynamicznie).
    - Destruktor zwalnia zaalokowany bufor (`delete content_bytes`).
    - Interfejs używa surowych wskaźników (`new`/`delete`) oraz zwraca surowe `File*` w `saveFileAs`
  - **Użycie w projekcie**: `main.cpp` używa `File` do wczytywania kluczy i plików wejściowych oraz do zapisu wyników (przykładowo przy operacjach Blowfish i RSA).

- **WASM Wrapper** - `src/wasm/wasm_wrapper.cpp` zawiera funkcje `extern "C"` do eksportu do WebAssembly, które przyjmują wskaźniki i rozmiary buforów, konwertują je na `std::vector<uint8_t>`, wywołują odpowiednie algorytmy, a następnie kopiują wynik z powrotem do bufora wyjściowego. Ten kod jest przeznaczony do demonstracji działania algorytmów w środowisku przeglądarki i jest budowany przy pomocy Emscripten (`emcc`), co pozwala na łatwe testowanie i prezentację działania algorytmów bez konieczności instalowania lokalnego środowiska C++.


**5. CLI**

Implementacja interfejsu wiersza poleceń zlokalizowana w `src/main.cpp` korzysta z biblioteki `CLI11` i wzorca subkomend (`blowfish`, `rsa`, `md5`).

Dla każdej subkomendy zdefiniowano logiczne grupy opcji (`mode`, `key`, `input`) z walidacją (`CLI::ExistingFile`, `excludes`, `require_option`) i callbackami wykonującymi rzeczywistą logikę. Callbacki używają klasy `File` do odczytu/zapisu bajtów, wywołują odpowiednie klasy algorytmów (`Blowfish`, `RSA`, `MD5`) i obsługują wynik (zapis do pliku lub wypis na stdout). Błędy walidacji zgłaszane są przez `CLI::ValidationError`, a opcja `-v/--verbose` włącza dodatkowe komunikaty diagnostyczne. 

Interfejs został zaprojektowany tak, aby można było łatwo używać go w skryptach, gdyż obsługuje zarówno wejście/wyjście z plików, jak i bezpośrednio z konsoli.


**6. Makefile**

Makefile definiuje ***targety*** zarówno dla budowania całego projektu, jak i dla każdego algorytmu osobno, a także dla testów i demo WebAssembly. Pliki źródłowe są najpierw kompilowane do obiektów, a następnie linkowane do finalnego binarnego `cryptotool` (lub poszczególnych testów).

**7. Ograniczenia i propozycje ulepszeń**
- **Bezpieczeństwo**: Implementacje nie są produkcyjnie bezpieczne — MD5 jest kryptograficznie złamany, RSA-small używa zbyt krótkich kluczy oraz nieużywa żadnego schematu wypełnienia (np. OAEP), a RNG nie jest kryptograficznie bezpieczne.
- **Możliwe ulepszenia**:
  - zastąpić `RNG` bezpiecznym generatorem (np. `std::random_device` + `std::mt19937_64` lub biblioteka kryptograficzna),
  - użyć biblioteki wielkiej arytmetyki (np. `boost::multiprecision::cpp_int`) aby zaimplementować RSA o realnych rozmiarach kluczy,

**8. Instrukcje uruchomienia i testy**
- Szczegóły budowania i użycia znajdują się w [README.md](README.md). Dla demonstracji dostępne jest także demo webowe budowane przez Emscripten (`make build_wasm`), co pozwala na uruchomienie kodu w przeglądarce na dowolnej platformie oraz interfejs CLI opisany w README.
- Testy jednostkowe znajdują się w katalogu `tests/` (np. [tests/blowfish/test_blowfish.cpp](tests/blowfish/test_blowfish.cpp)). Można je uruchomić za pomocą `make test` lub bezpośrednio wykonując skompilowane pliki testowe (np. `./tests/blowfish/test_blowfish`).

