#!/usr/bin/env bash
# C++17 required. Sources: ./C++ Code/*.cpp  |  Binaries: ./bin/
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pedantic)
DIR="$(cd "$(dirname "$0")" && pwd)"
CODE="$DIR/C++ Code"
BIN="$DIR/bin"
mkdir -p "$BIN"

echo "Building Exception_Handling (C++17)..."
for src in "$CODE"/*.cpp; do
  base="$(basename "$src" .cpp)"
  echo "  $base.cpp -> bin/$base"
  "$CXX" "${FLAGS[@]}" "$src" -o "$BIN/$base"
done
echo ""
echo "Sources : $CODE/"
echo "Run     : $BIN/01_basics_try_catch"
echo "All     : for f in $BIN/*; do echo \"=== \$f ===\"; \"\$f\"; done"
