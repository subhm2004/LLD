#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building concurrent_hashmap_app (C++17)..."
"$CXX" "${FLAGS[@]}" main.cpp -o concurrent_hashmap_app

# clangd: one TU entry (headers inherit these flags via includes)
ROOT="$(pwd)"
cat > compile_commands.json <<EOF
[
  {
    "directory": "${ROOT}",
    "command": "${CXX} -std=c++17 -Wall -Wextra -pthread -I${ROOT} -c main.cpp",
    "file": "${ROOT}/main.cpp"
  }
]
EOF

echo "Run: ./concurrent_hashmap_app"
