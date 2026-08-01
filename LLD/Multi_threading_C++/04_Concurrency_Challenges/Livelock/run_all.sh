#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

./compile.sh

for exe in bin/0*; do
  echo ""
  echo "========== $(basename "$exe") =========="
  "$exe"
done

echo ""
echo "All livelock demos finished."
