#!/usr/bin/env bash
# Build all Multi_threading_C++ modules that have compile.sh
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

MODULES=(
  "01_Fundamentals"
  "02_Concurrency_Patterns/Signaling_Pattern"
  "02_Concurrency_Patterns/Thread_Pool_Pattern"
  "02_Concurrency_Patterns/Producer_Consumer_Pattern"
  "02_Concurrency_Patterns/Reader_Writer_Pattern"
  "03_Lock_Free/Compare_And_Swap"
  "04_Concurrency_Challenges/Deadlock"
  "04_Concurrency_Challenges/Livelock"
  "06_Interview_Problems/Fizz_Buzz"
  "06_Interview_Problems/Merge_Sort"
  "06_Interview_Problems/Print_in_Order"
  "06_Interview_Problems/Print_FooBar_Alternately"
  "06_Interview_Problems/Print_Zero_Even_Odd"
  "06_Interview_Problems/Building_H2O"
  "06_Interview_Problems/Barrier_Synchronization"
  "06_Interview_Problems/Bounded_Blocking_Queue"
  "06_Interview_Problems/Dining_Philosophers_LC1226"
  "06_Interview_Problems/Web_Crawler_Multithreaded_LC1242"
)

echo "Building Multi_threading_C++ modules from $ROOT"
echo ""

for dir in "${MODULES[@]}"; do
  if [[ -x "$dir/compile.sh" ]]; then
    echo ">>> $dir"
    (cd "$dir" && ./compile.sh)
    echo ""
  else
    echo ">>> SKIP $dir (no compile.sh)"
  fi
done

echo "Done. Classic problems (05) compile manually — see 05_Classic_Problems/README.md"
