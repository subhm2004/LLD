#!/usr/bin/env bash
set -euo pipefail
CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building movie_ticket_app (C++17)..."
"$CXX" "${FLAGS[@]}" "main.cpp" -o "movie_ticket_app"
echo "Run: ./movie_ticket_app"
