#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/C++ Code"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra)

echo "=== L9 Factory Pattern — build all demos ==="

"$CXX" "${FLAGS[@]}" SimpleFactory.cpp -o simple_factory_demo
echo "OK: ./C++ Code/simple_factory_demo"

"$CXX" "${FLAGS[@]}" FactoryMethod.cpp -o factory_method_demo
echo "OK: ./C++ Code/factory_method_demo"

"$CXX" "${FLAGS[@]}" AbstractFactory.cpp -o abstract_factory_demo
echo "OK: ./C++ Code/abstract_factory_demo"

echo ""
echo "Run:"
echo "  ./C++ Code/simple_factory_demo"
echo "  ./C++ Code/factory_method_demo"
echo "  ./C++ Code/abstract_factory_demo"
