#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS="-std=c++17 -Wall -Wextra -pthread -O0"
mkdir -p bin

for src in *.cpp; do
  base="${src%.cpp}"
  echo "Compiling $src -> bin/$base"
  "$CXX" $FLAGS "$src" -o "bin/$base"
done

echo "All fundamentals built in bin/"
