# Simple Tetris C++ Edition

Comes with 3 binaries:
* tetris - reads stdin, writes to stdout as requested
* tetris_interactive - takes input and shows the board after every drop
* tetris_tests - verifies that the examples found in `input.txt` give the correct results

## Build locally

Prerequisites:
* c++ compiler (code was build using gcc 12.2, but anything that supports C++17 should work)
* CMake (anything non-ancient will likely do)

In the project dir:
1. `mkdir build`
2. `cd build`
3. `cmake .. -DCMAKE_BUILD_TYPE=Release`
4. `make`
5. `./tetris < ../input.txt > output.txt`
6. `cat output.txt`

## Use e.g. replit

1. create a new C++ repl
2. upload `board.hpp`, `shape.hpp`, `tetris.cpp`, and `input.txt`
3. rename `tetris.cpp` to `main.cpp`
4. build
5. switch to shell and run `./tetris < input.txt > output.txt`
6. `cat output.txt`
