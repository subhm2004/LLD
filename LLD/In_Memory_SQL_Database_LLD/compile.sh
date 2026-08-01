#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -I.)
echo "Building sql_database_app (C++17)..."
"$CXX" "${FLAGS[@]}" main.cpp -o sql_database_app
echo "Run: ./sql_database_app"
