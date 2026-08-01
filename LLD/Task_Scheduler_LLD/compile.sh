#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)
echo "Building task_scheduler_app (C++17)..."
"$CXX" "${FLAGS[@]}" main.cpp -o task_scheduler_app
echo "Run: ./task_scheduler_app"
