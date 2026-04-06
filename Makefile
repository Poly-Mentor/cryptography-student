build: build_blowfish build_rsa_small
	g++ src/main.cpp bin/blowfish.o bin/arithmetics.o bin/rng.o -o bin/app

run: build
	./bin/app

build_blowfish:
	g++ -c src/blowfish/blowfish.cpp -o bin/blowfish.o

build_rsa_small:
	g++ -c src/rsa-small/arithmetics.cpp -o bin/arithmetics.o
	g++ -c src/rsa-small/rng.cpp -o bin/rng.o

build_blowfish_test: build_blowfish
	g++ -c tests/blowfish/test_blowfish.cpp -o bin/test_blowfish.o
	g++ bin/test_blowfish.o bin/blowfish.o -o bin/test_blowfish

run_blowfish_test: build_blowfish_test
	./bin/test_blowfish

build_rsa_small_tests: build_rsa_small
	g++ -c tests/rsa-small/test_arithmetics.cpp -o bin/test_arithmetics.o
	g++ -c tests/rsa-small/test_rng.cpp -o bin/test_rng.o
	g++ bin/test_arithmetics.o bin/arithmetics.o -o bin/test_arithmetics
	g++ bin/test_rng.o bin/rng.o -o bin/test_rng

run_rsa_small_tests: build_rsa_small_tests
	./bin/test_arithmetics
	./bin/test_rng


build_tests: build_blowfish_test build_rsa_small_tests
	

run_tests: build_tests
	./bin/test_*

clean:
	rm -rf bin/*
