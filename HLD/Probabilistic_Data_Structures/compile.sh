#!/usr/bin/env bash
# ============================================================================
#  compile.sh — saare probabilistic data structure demos build karo
# ----------------------------------------------------------------------------
#  Chalao:
#     ./compile.sh          # sab build karo
#     ./compile.sh 4        # sirf demo 4 build karke CHALA do
#     ./compile.sh compare  # sabka muqabla (05)
#     ./compile.sh clean    # binaries hatao
#
#  ⚠ -O2 lagaya hai kyunki ye demos lakhon-crore items process karte hain.
#     Bina optimization ke kaafi dheere chalenge.
# ============================================================================
set -euo pipefail

cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -O2 -pthread -I.)

declare -a DEMOS=(
    "01_bloom_filter.cpp"
    "02_counting_bloom_filter.cpp"
    "03_count_min_sketch.cpp"
    "04_hyperloglog.cpp"
    "05_compare_all.cpp"
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
    build "05_compare_all.cpp"
    echo "Running:"
    echo
    ./05_compare_all_demo
    ;;

[1-5])
    src="${DEMOS[$(($1 - 1))]}"
    build "$src"
    echo "Running:"
    echo
    "./${src%.cpp}_demo"
    ;;

all)
    echo "Building saare demos (C++17, -O2)..."
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
    echo "Pata nahi '$1' kya hai. Use karo: ./compile.sh [all|1-5|compare|clean]" >&2
    exit 1
    ;;
esac
