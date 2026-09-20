#!/usr/bin/env bash
set -euo pipefail

mkdir -p build/
cmake -S . -B build -G Ninja \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=Debug
CLICOLOR_FORCE=1 cmake --build build --parallel 4 \
  | tee >(sed -r "s/\x1B\[([0-9]{1,3}(;[0-9]{1,3})*)?[mGK]//g" > build.log)

echo "----------------------------------------"

./build/etudes "$@"