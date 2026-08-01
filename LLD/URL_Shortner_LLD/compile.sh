#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building url_shortner_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "url_shortner_app"
echo "Run: ./url_shortner_app"
