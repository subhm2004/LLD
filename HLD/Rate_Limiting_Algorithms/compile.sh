#!/usr/bin/env bash
# ============================================================================
#  compile.sh — saare rate limiting demos build karo
# ----------------------------------------------------------------------------
#  Chalao:
#     ./compile.sh          # sab build karo
#     ./compile.sh 3        # sirf demo 3 build karke CHALA do
#     ./compile.sh compare  # sabka muqabla (06)
#     ./compile.sh clean    # binaries hatao
# ============================================================================
set -euo pipefail

cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)

declare -a DEMOS=(
    "01_fixed_window.cpp"
    "02_sliding_window_log.cpp"
    "03_sliding_window_counter.cpp"
    "04_token_bucket.cpp"
    "05_leaking_bucket.cpp"
    "06_compare_all.cpp"
)

build() {
    local src="$1"
    local out="${src%.cpp}_demo"
    echo "  building $src"
    "$CXX" "${FLAGS[@]}" "$src" -o "$out"
}

case "${1:-all}" in
clean)
    rm -f ./*_demo
    echo "Saaf ho gaya."
    ;;

compare)
    build "06_compare_all.cpp"
    echo "Running:"
    echo
    ./06_compare_all_demo
    ;;

[1-6])
    src="${DEMOS[$(($1 - 1))]}"
    build "$src"
    echo "Running:"
    echo
    "./${src%.cpp}_demo"
    ;;

all)
    echo "Building saare demos (C++17)..."
    for src in "${DEMOS[@]}"; do
        build "$src"
    done
    echo
    echo "Ho gaya. Padhne ka sahi order:"
    for src in "${DEMOS[@]}"; do
        echo "   ./${src%.cpp}_demo"
    done
    ;;

*)
    echo "Pata nahi '$1' kya hai. Use karo: ./compile.sh [all|1-6|compare|clean]" >&2
    exit 1
    ;;
esac
