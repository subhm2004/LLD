#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS="-std=c++17 -Wall -Wextra -pthread -O0 -I. -I.."
mkdir -p bin

echo "Compiling DiningPhilosophers.cpp"
"$CXX" $FLAGS -c DiningPhilosophers.cpp -o DiningPhilosophers.o

for src in 0*.cpp; do
  base="${src%.cpp}"
  echo "Compiling $src -> bin/$base"
  "$CXX" $FLAGS "$src" DiningPhilosophers.o -o "bin/$base"
done

echo "Dining Philosophers LC1226 demos built in bin/"
