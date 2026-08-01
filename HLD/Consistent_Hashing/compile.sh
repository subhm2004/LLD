#!/usr/bin/env bash
# ============================================================================
#  compile.sh — saare consistent hashing demos build karo
# ----------------------------------------------------------------------------
#  Chalao:
#     ./compile.sh          # sab build karo
#     ./compile.sh 3        # sirf step 3 build karke CHALA do
#     ./compile.sh main     # capstone demo build karke chala do
#     ./compile.sh clean    # saari binaries hatao
# ============================================================================
set -euo pipefail

cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)

# step number -> source file
declare -a STEPS=(
    "01_modulo_hashing_problem.cpp"
    "02_hash_ring_basic.cpp"
    "03_virtual_nodes.cpp"
    "04_replication.cpp"
    "05_bounded_loads.cpp"
    "06_rendezvous_hashing.cpp"
)

build() {
    local src="$1"
    local out="${src%.cpp}_demo"
    echo "  building $src"
    "$CXX" "${FLAGS[@]}" "$src" -o "$out"
}

case "${1:-all}" in
clean)
    rm -f ./*_demo consistent_hashing_app
    echo "Saaf ho gaya."
    ;;

main)
    echo "Building capstone (C++17)..."
    "$CXX" "${FLAGS[@]}" main.cpp -o consistent_hashing_app
    echo "Running:"
    echo
    ./consistent_hashing_app
    ;;

[1-6])
    src="${STEPS[$(($1 - 1))]}"
    build "$src"
    echo "Running:"
    echo
    "./${src%.cpp}_demo"
    ;;

all)
    echo "Building saare demos (C++17)..."
    for src in "${STEPS[@]}"; do
        build "$src"
    done
    echo "  building main.cpp (capstone)"
    "$CXX" "${FLAGS[@]}" main.cpp -o consistent_hashing_app
    echo
    echo "Ho gaya. Ab chalao (order me padhna sabse achha):"
    for src in "${STEPS[@]}"; do
        echo "   ./${src%.cpp}_demo"
    done
    echo "   ./consistent_hashing_app     # capstone"
    ;;

*)
    echo "Pata nahi '$1' kya hai. Use karo: ./compile.sh [all|1-6|main|clean]" >&2
    exit 1
    ;;
esac
