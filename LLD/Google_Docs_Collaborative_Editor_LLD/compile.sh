#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -I.)
echo "Building collab_editor_app (C++17)..."
"$CXX" "${FLAGS[@]}" main.cpp -o collab_editor_app
echo "Run: ./collab_editor_app"
