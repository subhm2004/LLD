#!/usr/bin/env bash
# ============================================================================
#  compile.sh — saare cache eviction policy demos build karo
# ----------------------------------------------------------------------------
#  Chalao:
#     ./compile.sh          # sab build karo
#     ./compile.sh 4        # sirf demo 4 build karke CHALA do
#     ./compile.sh compare  # sabka muqabla (05)
#     ./compile.sh clean    # binaries hatao
#
#  ⚠ -O2 zaroori hai — ye demos lakhon cache operations chalate hain
#     (aur Belady optimal to poora trace do baar dekhta hai).
# ============================================================================
set -euo pipefail

cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -O2 -pthread -I.)

declare -a DEMOS=(
    "01_lru.cpp"
    "02_lfu.cpp"
    "03_fifo_random_clock.cpp"
    "04_arc.cpp"
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
