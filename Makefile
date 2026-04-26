build: build_blowfish build_rsa_small build_rsa_small
	g++ src/main.cpp bin/blowfish.o bin/arithmetics.o bin/rng.o -o bin/app

run: build
	./bin/app

build_blowfish:
	g++ -c src/blowfish/blowfish.cpp -o bin/blowfish.o

build_rsa_small:
	g++ -c src/rsa-small/arithmetics.cpp -o bin/arithmetics.o
	g++ -c src/rsa-small/rng.cpp -o bin/rng.o
	g++ -c src/rsa-small/rsa-small.cpp -o bin/rsa-small.o

build_md5:
	g++ -c src/md5/md5.cpp -o bin/md5.o

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
	g++ -c tests/rsa-small/test_rsa-small.cpp -o bin/test_rsa-small.o
	g++ bin/test_rsa-small.o bin/rsa-small.o bin/arithmetics.o bin/rng.o -o bin/test_rsa-small

run_rsa_small_tests: build_rsa_small_tests
	./bin/test_arithmetics
	./bin/test_rng
	./bin/test_rsa-small

build_md5_test: build_md5
	g++ -c tests/md5/test_md5.cpp -o bin/test_md5.o
	g++ bin/test_md5.o bin/md5.o -o bin/test_md5

run_md5_test: build_md5_test
	./bin/test_md5

build_tests: build_blowfish_test build_rsa_small_tests build_md5_test
	

run_tests: build_tests
	./bin/test_*

clean:
	rm -rf bin/*
