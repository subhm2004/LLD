#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT/06_Interview_Problems"

MODULES=(
  "Fizz_Buzz"
  "Merge_Sort"
  "Print_in_Order"
  "Print_FooBar_Alternately"
  "Print_Zero_Even_Odd"
  "Building_H2O"
  "Barrier_Synchronization"
  "Bounded_Blocking_Queue"
  "Dining_Philosophers_LC1226"
  "Web_Crawler_Multithreaded_LC1242"
)

echo "Building 06_Interview_Problems from $ROOT"
for dir in "${MODULES[@]}"; do
  echo ">>> $dir"
  (cd "$dir" && chmod +x compile.sh && ./compile.sh)
  echo ""
done
echo "All interview problems built."
