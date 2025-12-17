#!/usr/bin/env bash
set -e

echo "Formatting code..."
clang-format -i *.c *.cpp *.h *.hpp 2>/dev/null || true

echo "Compiling..."
g++ -std=c++20 -Wall -Wextra -Wpedantic -O2 -DLEXER_TEST lexer.cpp  -o lexer


echo "Build successful."
