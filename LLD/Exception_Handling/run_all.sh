#!/usr/bin/env bash
set -euo pipefail
DIR="$(cd "$(dirname "$0")" && pwd)"
"$DIR/compile.sh"
echo ""
for f in "$DIR"/bin/*; do
  echo "========== $(basename "$f") =========="
  "$f"
  echo ""
done
