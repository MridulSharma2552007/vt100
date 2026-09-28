all:
	cmake -S . -B build
	cmake --build build

run: all
	./build/main

clean:
	rm -rf build