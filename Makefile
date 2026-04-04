build_src:
	g++ src/main.cpp src/blowfish/**.cpp src/rsa-small/**.cpp -o bin/app
run:
	./bin/app
build_tests:
	g++ tests/blowfish/test_blowfish.cpp -o bin/test_blowfish
	g++ test/rsa-small/test-rsa/test_arithmetics.cpp -o bin/test_arithmetics
	g++ test/rsa-small/test-rsa/test_rng.cpp -o bin/test_rng
run_tests:
	./bin/test_*
