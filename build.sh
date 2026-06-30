#!/usr/bin/env bash
set -e

DEBUG_FLAGS=()

if [[ "${1:-}" == "-debug"  || "${1:-}" == "-d" ]]; then
  echo "Debug output enabled..."
  DEBUG_FLAGS=(
    -DLEXER_TEST
    -DPARSER_TEST
    -DSEMANTIC_TEST
  )
elif [[ -n "${1:-}" ]]; then
  echo "Unknown option: $1"
  echo "Usage: ./build.sh [-debug]"
  exit 1
fi


if [[ "${1:-}" == "-debug"  || "${1:-}" == "-d" ]]; then
    echo "Formatting code..."
fi

clang-format -i *.c *.cpp *.h *.hpp 2>/dev/null || true



if [[ "${1:-}" == "-debug"  || "${1:-}" == "-d" ]]; then
    echo "Compiling..."
fi

#g++ -std=c++20 -Wall -Wextra -Wpedantic -O0 -g -DLEXER_TEST -DPARSER_TEST -DSEMANTIC_TEST lexer.cpp parser.cpp node.cpp main.cpp semantic.cpp -o compiler
g++ -std=c++20 -Wall -Wextra -Wpedantic -O0 -g "${DEBUG_FLAGS[@]}" lexer.cpp parser.cpp node.cpp main.cpp semantic.cpp -o compiler

./compiler sample.c 


if [[ "${1:-}" == "-debug"  || "${1:-}" == "-d" ]]; then
    echo "Build successful."
fi

