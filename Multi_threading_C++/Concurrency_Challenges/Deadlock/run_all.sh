#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

./compile.sh

for exe in bin/0[1-6]*; do
  echo ""
  echo "========== $(basename "$exe") =========="
  "$exe"
done

echo ""
echo "Skipped bin/07_optional_infinite_deadlock (manual only)."
echo "All safe deadlock demos finished."
