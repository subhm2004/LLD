#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

OUT="${1:-cor_demo}"
SRC="main.cpp"

echo "Compiling $SRC -> $OUT"
g++ -std=c++17 -Wall -Wextra -o "$OUT" "$SRC"
echo "Run: ./$OUT"
