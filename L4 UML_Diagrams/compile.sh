#!/usr/bin/env bash
set -euo pipefail

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pedantic)
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CODE="$DIR/C++ Code"
BIN="$DIR/bin"
mkdir -p "$BIN"

echo "Building L4 UML_Diagrams (C++17)..."
for src in "$CODE"/*.cpp; do
  base="$(basename "$src" .cpp)"
  echo "  $base -> bin/$base"
  "$CXX" "${FLAGS[@]}" "$src" -o "$BIN/$base"
done
echo ""
echo "Run examples:"
echo "  $BIN/01_Inheritance_Five_Types"
echo "  $BIN/02_Composition_UniquePtr"
echo "  $BIN/03_Composition_OldStyle_Ptr"
echo "  $BIN/04_Composition_Chair_Example"
