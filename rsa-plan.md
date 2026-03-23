# Plan implementacji RSA — opis elementów do zaimplementowania (language-agnostic)

Cel: dostarczyć kompletny, krok-po-kroku plan implementacji algorytmu RSA od podstaw — bez kodu, z wyjaśnieniem poszczególnych komponentów, które trzeba zaimplementować i przetestować.

Jest to projekt studencki, edukacyjny. Będzie implementowany w C++ (z opcją użycia biblioteki bignum, np. Botan::BigInt), ale plan jest napisany w sposób niezależny od języka, żeby nie podpowiadać konkretnych rozwiązań implementacyjnych i w ten sposób zachęcać do samodzielnego myślenia o strukturze kodu i algorytmach oraz stanowić trening programowania w C++.

**1. Przegląd i założenia**
- Opis: RSA to asymetryczny system klucza publicznego oparty na trudności faktoryzacji dużych liczb całkowitych. Główne operacje to: generowanie pary kluczy (publiczny, prywatny), szyfrowanie/odszyfrowywanie, podpisywanie/weryfikacja.
- Zakres implementacji: generowanie kluczy, operacje modularne, bezpieczeństwo (padding, losowość), optymalizacje (CRT), testy, interfejs API i formaty przechowywania kluczy.

**2. Matematyczne podstawy (co zaimplementować)**
- Typy liczb i precyzja: potrzeba implementacji lub użycia arytmetyki dużych liczb całkowitych (big integers / bignum) obsługującej operacje: dodawanie, odejmowanie, mnożenie, dzielenie z resztą, porównania, bitowe przesunięcia i konwersje (do/z bajtów).
- Operacje modularne: modularne dodawanie, mnożenie, potęgowanie (modular exponentiation) i modularne odwrotności.
- Algorytm Euklidesa rozszerzonego (Extended Euclidean Algorithm) do obliczenia odwrotności modularnej (dla obliczenia d = e^{-1} mod phi(n)).
- Test pierwszości: implementacja testu prawdopodobnego pierwszości (np. Miller–Rabin) oraz opcjonalnie prostych testów wstępnych (podział przez małe liczby pierwsze) dla szybszego odrzucania złożonych kandydatów.

**3. Generator losowych i kryptograficzny PRNG**
- Wybór i interfejs: potrzebny deterministyczny generator losowy to nie jest wystarczające — wymagamy CSPRNG (cryptographically secure PRNG) z systemu operacyjnego lub dobrze przetestowanego źródła losowości.
- Użycie: generowanie kandydatów na liczby pierwsze, losowe wartości do pól paddingu i opcjonalne blinding w dekrypcji.
- API: funkcja zwracająca żądane liczby losowych bajtów/bitów.

**4. Generacja liczb pierwszych i klucza**
- Parametry rozmiaru: określ rozmiary kluczy (np. 2048, 3072, 4096 bitów). Zdefiniuj politykę wyboru rozmiarów p i q (zwykle podobnej długości, ale p != q oraz odległość nie za mała).
- Generowanie kandydata: generuj losową liczbę odd, z ustawionymi bitami (najwyższy bit = 1) aby zapewnić odpowiednią długość.
- Wstępna filtracja: sprawdź podzielność przez małe liczby pierwsze dla szybkości.
- Test Miller–Rabin: wykonaj wystarczającą liczbę rund testu probabilistycznego by osiągnąć akceptowalne prawdopodobieństwo bycia pierwszą.
- Wybór p i q: po znalezieniu dwóch odrębnych liczb pierwszych: oblicz n = p * q oraz phi(n) = (p-1)*(q-1) lub Carmichael lambda(n) = lcm(p-1, q-1) (omów zalety wyboru phi vs lambda).
- Wybór wykładnika publicznego `e`: zwykle mała stała o niskiej wadze 1s (np. 65537) — opisać kryteria: gcd(e, phi) = 1.
- Obliczenie wykładnika prywatnego `d`: oblicz modularną odwrotność e modulo phi (lub lambda). Wykorzystaj rozszerzony algorytm Euklidesa.
- Weryfikacja: sprawdź, że e * d ≡ 1 (mod phi) i że p oraz q nie są zbyt bliskie (zapobiegać atakom).

**5. Reprezentacja kluczy i formaty**
- Zawartość klucza publicznego: (n, e).
- Zawartość klucza prywatnego minimalna: (n, d) i rozszerzona: (p, q, d mod (p-1), d mod (q-1), q^{-1} mod p) — wartości potrzebne do optymalizacji CRT.
- Serializacja: format zapisu bajtowego (big-endian liczby) i meta-informacje (rozmiar, algorytm). Zasugerować mechanizmy eksportu/importu (PEM/DER) ale bez implementowania standardów: opisać co trzeba zapisać i jakie są konsekwencje interoperacyjności.
- Zabezpieczenie kluczy prywatnych: szyfrowanie pliku klucza prywatnego, ograniczenia dostępu, i bezpieczne czyszczenie pamięci.

**6. Operacje podstawowe: szyfrowanie, odszyfrowywanie, podpisywanie, weryfikacja**
- Szyfrowanie: C = M^e mod n — opis wymagań co do reprezentacji wiadomości M (konwersja do liczby < n), konieczność stosowania paddingu.
- Odszyfrowanie: M = C^d mod n — opis podstawowej ścieżki i możliwych błędów (np. nieprawidłowy padding).
- Podpisywanie: S = H(M)^d mod n — opis jak użyć skrótu i formatów (np. wtórna procedura podpisu oparta na hashie i paddingu).
- Weryfikacja: sprawdzenie S^e mod n == oczekiwany bloki (odkodowany hash + padding).

**7. Padding i bezpieczeństwo semantyczne**
- Dlaczego padding: RSA bez paddingu jest deterministyczne i podatne na ataki (np. chosen-plaintext). Należy zapewnić semantyczne bezpieczeństwo (IND-CPA) oraz ochronę podpisów.
- Omówienie schematów: PKCS#1 v1.5 (historyczny, proste reguły) i OAEP (bezpieczniejszy, oparty na MGF i funkcjach haszujących) — opisać konceptualne kroki OAEP (mask generation, XOR) bez implementacji kodu.
- Implementacyjne punkty: funkcje hash, MGF, format danych, walidacja paddingu przy odszyfrowaniu i obsługa błędów w sposób, który nie ujawnia informacji (ważne dla odporności na side-channel i oracle attacks).

**8. Efektywność: modular exponentiation i CRT**
- Potęgowanie modularne: implementacja efektywna (square-and-multiply) z okrężnymi ulepszeniami (montgomery multiplication jako opcja) — opisać interfejs i wymagania testowe.
- Optymalizacja CRT: jak przechowywać dp = d mod (p-1), dq = d mod (q-1), qinv = q^{-1} mod p; procedura odbudowywania wyniku i korzyści wydajnościowe (~4x szybciej).
- Weryfikacja poprawności: testy porównawcze między wersją bez CRT i z CRT.

**9. Ochrona przed atakami i podatnościami**
- Blinding: opis idei (mnożenie wejścia przez losowy r^e mod n przed odszyfrowaniem i usunięcie maski po) i kiedy go używać.
- Constant-time: wskazać, które operacje muszą być wykonywane w stałym czasie (porównania, operacje pętli zależne od tajnych danych) i jak minimalizować boczne kanały (cache, branch prediction).
- Sprawdzanie integralności parametrów: weryfikacja, czy p i q są pierwsze, czy n ma poprawny rozmiar, czy wartości CRT są zgodne.
- Walidacja wejścia: odrzucanie niepoprawnych długości, monitorowanie wyjątków, bezpieczna obsługa błędów (nie wyciekają różnice w czasie czy treści błędu).

**10. Testy i walidacja**
- Jednostkowe: testy bignum (dodawanie, mnożenie, dzielenie, konwersje), testy modularnego potęgowania, testy algorytmu Euklidesa, testy Miller–Rabin z wewnętrznymi wektorami.
- Integracyjne: generacja pary kluczy, szyfrowanie-dekrypcja par próbek (różne rozmiary wiadomości), podpis-weryfikacja dla różnych długości kluczy.
- Testy negatywne: nieprawidłowe paddingi, niepoprawne klucze, graniczne rozmiary wiadomości.
- Porównania referencyjne: porównaj wyniki z istniejącymi bibliotekami (np. openssl) dla tego samego wejścia aby weryfikować zgodność (bez ujawniania implementacji wewnętrznej).

**11. API, interfejsy i UX programistyczny**
- Moduły/funkcje które warto wyróżnić: generator kluczy (parametry rozmiaru, e), moduł bignum, CSPRNG adapter, moduł paddingu, operacje publiczne (encrypt, decrypt, sign, verify), serializacja/parsowanie kluczy.
- Interfejsy: jasno określ wejście/wyjście w jednostkach bajtów lub liczb (np. bajty big-endian), dokumentuj wymagane pre- i post-warunki.

**12. Wydajność i profiling**
- Mierzenie: zbuduj małe benchmarki: generacja kluczy, czas szyfrowania/odszyfrowania, porównanie CRT vs non-CRT.
- Ulepszenia: opcje akceleracji (Montgomery, assembly, wielowątkowa generacja dużych liczb pierwszych).

**13. Przykładowy plan wdrożeniowy (zadania programistyczne)**
1. Implementacja modułu bignum (lub integracja z bezpieczną biblioteką bignum) i testy jednostkowe.
2. Implementacja CSPRNG-adaptera i testy entropii/wyjścia.
3. Implementacja testu pierwszości (Miller–Rabin) i filtrów.
4. Implementacja Euklidesa rozszerzonego i modularnej odwrotności.
5. Implementacja modular exponentiation oraz testy porównawcze.
6. Implementacja generatora kluczy (p,q,n,phi,e,d) i testy walidacyjne.
7. Implementacja paddingu OAEP i PKCS#1 v1.5 (opis implementacji; OAEP preferowany).
8. Implementacja funkcji szyfrowania/odszyfrowywania i podpisywania/weryfikacji.
9. Implementacja optymalizacji CRT i blindingu.
10. Testy integracyjne, dokumentacja API, serializacja kluczy.

**14. Najczęstsze pułapki i wskazówki praktyczne**
- Nigdy nie używaj niekryptograficznego PRNG do generacji kluczy.
- Zadbaj o walidację paddingu i równomierną obsługę błędów (nie ujawniać side-channelów przez różne komunikaty).
- Upewnij się, że p i q nie są zbyt bliskie i że n ma oczekiwany rozmiar bitowy.
- Rozważ użycie sprawdzonej biblioteki bignum zamiast pisania od zera, jeśli celem nie jest nauka implementacji bignum.

---
Jeśli chcesz, mogę teraz:
- wygenerować listę testów jednostkowych na podstawie powyższego planu, lub
- przygotować szkielet plików i interfejsów (nagłówki/kontrakty funkcji) bez implementacji logiki.

## 15. Prototypowanie: demonstracyjny "small-int" → migracja do bignum

Cel: szybko zweryfikować logikę algorytmiczną (potęgowanie modularne, EEA, Miller–Rabin, padding) przy użyciu prostszych typów (`long`), a następnie w prosty sposób przejść do prawdziwej arytmetyki wielkiej (`Botan::BigInt` lub inna biblioteka).

Krótki opis podejścia:
- Etap A — prototyp funkcjonalny (small-int): zaimplementuj algorytmy używając natywnych typów całkowitych (`long`) tak, żeby łatwo debugować i walidować logikę.
- Etap B — abstrakcja arytmetyki: zdefiniuj interfejs `BigIntLike` (operacje: add, sub, mul, divmod, mod_pow, inv_mod, randomize, from_bytes, to_bytes).
- Etap C — adaptery: stwórz `SmallIntAdapter` (wrap dla `long`) oraz `BotanBigIntAdapter` (używający `Botan::BigInt`).
- Etap D — migracja testów i optymalizacje: uruchom testy porównawcze i dopracuj backend bignum (CRT, blinding, Montgomery).

Plusy takiego podejścia:
- Szybkie prototypowanie i walidacja algorytmów bez komplikacji bignum.
- Łatwiejsze debugowanie i edukacja — zachowanie krok po kroku na małych liczbach.
- Możliwość równoległej implementacji i porównań (wyniki i wydajność).

Minusy / ryzyka:
- Prototyp może dawać fałszywe poczucie bezpieczeństwa (przepełnienia, brak CT, inne zachowanie arytmetyki).
- Optymalizacje i błędy specyficzne dla wielowyrazowych implementacji ujawnią się dopiero po migracji.
- Prototyp nie nadaje się do produkcyjnego użytku.

Rekomendacje praktyczne:
- Od samego początku używaj CSPRNG do generowania kandydatów (nie `rand()`).
- Trzymaj logikę algorytmiczną oddzieloną od implementacji arytmetyki przez `BigIntLike`.
- Dla każdego testu na `SmallIntAdapter` dodaj analogiczny test na `BotanBigIntAdapter` (regresja/porównanie wyników).
- Po migracji wykonaj audyt constant-time i dodaj blinding w backendzie bignum.

Plan migracji (konkretne kroki):
1. Implementacja prototypu (small-int): generator kluczy (na małych bitach), Miller–Rabin, modular exponentiation, EEA, OAEP (funkcjonalny).
2. Wprowadzenie `BigIntLike` i zaimplementowanie `SmallIntAdapter` (wrap nad `long`).
3. Implementacja `BotanBigIntAdapter` używającego `Botan::BigInt`.
4. Uruchomienie zestawu testów porównawczych: szyfrowanie/odszyfrowanie, podpis/weryfikacja, testy negatywne.
5. Dopracowanie backendu bignum: CRT, blinding, ewentualne optymalizacje Montgomery.

Proponowane testy porównawcze:
- Porównanie wyników enc/dec i sign/verify dla zestawu losowych wejść między `SmallIntAdapter` i `BotanBigIntAdapter`.
- Testy brzegowe: największe dopuszczalne wiadomości, złe paddingi, niepoprawne klucze.
- Property tests: losowe p,q z obu backendów dają zgodne n,e,d (matematycznie równoważne) dla tych samych parametrów.

Krótka uwaga bezpieczeństwa:
- Nigdy nie używaj prototypu do generowania kluczy produkcyjnych ani do przetwarzania prawdziwych danych. Prototyp służy wyłącznie do nauki i walidacji logiki.

Jeśli chcesz, mogę teraz automatycznie wygenerować szkic interfejsu `BigIntLike` oraz pliki szkieletowe `SmallIntAdapter` i `BotanBigIntAdapter`.

## Wnioski i mapa integracji z Botan

**Wnioski (Botan jako rekomendacja):**
- **Wniosek:** Botan jest dobrym wyborem dla projektu edukacyjno-praktycznego — łączy bezpieczne, przetestowane implementacje (`BigInt`, CSPRNG, RSA/PKCS#1, OAEP/PSS, serializację PEM/DER) z wygodnym C++ API.
- **Zalety:** `Botan::BigInt` (randomize/serialize/encode), wbudowane generowanie kluczy RSA (`RSA_PrivateKey(RNG, bits, exp)`), wygodne operacje szyfrowania/podpisywania, `secure_vector` dla tajnych danych, obsługa OAEP/PSS i BER/DER/PKCS#8.
- **Uwagi:** Botan udostępnia CRT/blinding/optimizacje i bezpieczne implementacje — to dobrze do porównań z własnymi modułami edukacyjnymi.
- **Rekomendacja integracji:** Użyj Botan do testów porównawczych i sanity-checków; implementuj edukacyjne moduły (bignum, Miller–Rabin, OAEP) i porównuj wyniki oraz wydajność z Botan.

### Mapa klas / funkcji Botan przypisana do etapów planu
- BigInt / arytmetyka bignum:
	- `Botan::BigInt` — konstrukcja, `randomize()`, `bits()`, `bytes()`, `serialize()/serialize_to()`, `from_bytes()`, `encode_1363()`, `mod_add()`, `mod_mul()`, `mul()`, `random_integer()` — użyj do testów arytmetyki i konwersji.
- CSPRNG:
	- `Botan::RandomNumberGenerator` (abstrakcja) — użyj `AutoSeeded_RNG` lub `system_rng` jako źródła losowości.
- Generowanie liczb pierwszych / testy pierwszości:
	- `Botan::random_prime()` / `Botan::is_miller_rabin_probable_prime()` / `Botan::is_prime()` — do porównań i walidacji.
- Generowanie kluczy RSA:
	- `Botan::RSA_PrivateKey(RandomNumberGenerator& rng, size_t bits, size_t exp = 65537)` — tworzy p,q,d,dp,dq,qinv i wykonuje sprawdzenia.
	- `Botan::RSA_PublicKey` — `get_n()`, `get_e()`, `public_key_bits()`.
	- `RSA_PrivateKey::private_key_bits()` / `Public_Key::public_key_bits()` — serializacja DER/PKCS#1.
- Operacje szyfrowania/dekrypcji/podpisu:
	- `PK_Ops::Encryption` / `PK_Ops::Decryption` / `PK_Ops::Signature` / `PK_Ops::Verification` — niskopoziomowe interfejsy; używaj wrapperów w `pubkey.h` i metod `create_*_op(...)` na kluczach.
	- Wywołania: `RSA_PrivateKey::create_decryption_op(rng, params, provider)`, `RSA_PublicKey::create_encryption_op(rng, params, provider)`, `create_signature_op`, `create_verification_op`.
- Padding / schematy:
	- Wybieraj schematy przez parametry op: np. `EME-OAEP(SHA-256)` lub `EMSA-PSS(SHA-256)` (Botan obsługuje OAEP i PSS; parametry ustawiane przy tworzeniu operacji).
- Optymalizacje i bezpieczeństwo:
	- `RSA_PrivateKey` udostępnia CRT-składniki: `get_d1()`, `get_d2()`, `get_c()` (q^{-1} mod p) oraz `check_key()`; Botan stosuje blinding/CT tam, gdzie to potrzebne.
- Serializacja / formaty:
	- `Botan::DER_Encoder`, `Botan::BER_Decoder`, `Private_Key::private_key_info()`, PKCS#8 helpery i `PEM`/`DER` API do eksportu/importu.
- Narzędzia pomocnicze:
	- `Botan::secure_vector<T>` — do bezpiecznego przechowywania tajnych danych.
	- `Botan::CT::*` i `BigInt::const_time_poison()` — pomocne przy implementacji odpornej na side-channels.

### Szybka lista metod RSA przydatnych do integracji
- `Botan::RSA_PrivateKey::RSA_PrivateKey(RandomNumberGenerator& rng, size_t bits, size_t exp=65537)`
- `Botan::RSA_PrivateKey::create_decryption_op(...)`, `create_signature_op(...)`
- `Botan::RSA_PublicKey::create_encryption_op(...)`, `create_verification_op(...)`
- `Botan::RSA_PrivateKey::get_p()/get_q()/get_d()/get_d1()/get_d2()/get_c()`
- `Botan::RSA_PublicKey::get_n()/get_e()/public_key_bits()`
- `Botan::BigInt::randomize()/from_bytes()/serialize()/encode_1363()/random_integer()/bits()/bytes()`


