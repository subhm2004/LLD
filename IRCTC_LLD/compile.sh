#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building irctc_app (C++17)..."
"$CXX" "${FLAGS[@]}" main.cpp -o irctc_app

# clangd / IDE: translation unit flags for this project
cat > "$ROOT/compile_commands.json" <<EOF
[
  {
    "directory": "$ROOT",
    "command": "$CXX -std=c++17 -pthread -I$ROOT -c main.cpp",
    "file": "$ROOT/main.cpp"
  }
]
EOF

echo "Run: ./irctc_app"
