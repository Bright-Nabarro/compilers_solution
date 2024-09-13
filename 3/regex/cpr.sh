#!/usr/bin/env bash
set -e

cmake -S . -B build # -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build --parallel 8

echo "build success"

