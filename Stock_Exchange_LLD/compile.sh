#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread)
echo "Building stock_exchange_app (C++17)..."
"$CXX" "${FLAGS[@]}" main.cpp -o stock_exchange_app

cat > "$ROOT/compile_commands.json" <<EOF
[
  {
    "directory": "$ROOT",
    "command": "$CXX -std=c++17 -pthread -c main.cpp",
    "file": "$ROOT/main.cpp"
  }
]
EOF

echo "Run: ./stock_exchange_app"
