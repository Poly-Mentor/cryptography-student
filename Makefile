build_src:
	g++ src/main.cpp src/blowfish/**.cpp src/rsa-small/**.cpp -o bin/app
run: build_src
	./bin/app
build_tests:
	g++ tests/blowfish/test_blowfish.cpp src/blowfish/blowfish.cpp -o bin/test_blowfish
	g++ tests/rsa-small/test_arithmetics.cpp src/rsa-small/arithmetics.cpp -o bin/test_arithmetics
	g++ tests/rsa-small/test_rng.cpp src/rsa-small/rng.cpp -o bin/test_rng
run_tests: build_tests
	./bin/test_*

clean:
	rm -rf bin/*
