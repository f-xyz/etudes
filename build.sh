#!/usr/bin/env bash
set -euo pipefail

mkdir -p build/
cmake -S . -B build -G Ninja \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel 4

echo "----------------------------------------"

./build/etudes "$@"