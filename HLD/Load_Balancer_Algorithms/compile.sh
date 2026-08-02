#!/usr/bin/env bash
# ============================================================================
#  compile.sh — saare load balancer algorithm demos build karo
# ----------------------------------------------------------------------------
#  Chalao:
#     ./compile.sh            # sab build karo
#     ./compile.sh static     # sirf static algorithms
#     ./compile.sh dynamic    # sirf dynamic algorithms
#     ./compile.sh clean      # saari binaries hatao
# ============================================================================
set -euo pipefail

cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -pthread -I.)

STATIC_SRC=(
    "Static_Algorithms/01_round_robin.cpp"
    "Static_Algorithms/02_weighted_round_robin.cpp"
    "Static_Algorithms/03_ip_hashing.cpp"
)

DYNAMIC_SRC=(
    "Dynamic_Algorithms/01_least_connections.cpp"
    "Dynamic_Algorithms/02_weighted_least_connections.cpp"
    "Dynamic_Algorithms/03_least_response_time.cpp"
    "Dynamic_Algorithms/04_resource_based.cpp"
)

build() {
    local src="$1"
    local out="${src%.cpp}_demo"
    echo "  building $src"
    "$CXX" "${FLAGS[@]}" "$src" -o "$out"
}

build_all() {
    for src in "$@"; do
        build "$src"
    done
}

case "${1:-all}" in
clean)
    rm -f Static_Algorithms/*_demo Dynamic_Algorithms/*_demo
    echo "Saaf ho gaya."
    ;;

static)
    echo "Building STATIC algorithms (C++17)..."
    build_all "${STATIC_SRC[@]}"
    ;;

dynamic)
    echo "Building DYNAMIC algorithms (C++17)..."
    build_all "${DYNAMIC_SRC[@]}"
    ;;

all)
    echo "Building saare demos (C++17)..."
    build_all "${STATIC_SRC[@]}" "${DYNAMIC_SRC[@]}"
    echo
    echo "Ho gaya. Padhne ka sahi order:"
    for src in "${STATIC_SRC[@]}" "${DYNAMIC_SRC[@]}"; do
        echo "   ./${src%.cpp}_demo"
    done
    ;;

*)
    echo "Pata nahi '$1' kya hai. Use karo: ./compile.sh [all|static|dynamic|clean]" >&2
    exit 1
    ;;
esac
