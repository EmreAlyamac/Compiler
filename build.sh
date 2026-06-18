#!/usr/bin/env bash
set -e

echo "Formatting code..."
clang-format -i *.c *.cpp *.h *.hpp 2>/dev/null || true

echo "Compiling..."
g++ -std=c++20 -Wall -Wextra -Wpedantic -O0 -g -DLEXER_TEST -DPARSER_TEST lexer.cpp parser.cpp node.cpp main.cpp -o compiler
./compiler sample.c


echo "Build successful."
