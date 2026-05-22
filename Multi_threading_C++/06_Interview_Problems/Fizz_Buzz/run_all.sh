#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

./compile.sh

for bin in bin/0*; do
  echo ""
  echo ">>>>>>>>>> $(basename "$bin") <<<<<<<<<<"
  "$bin"
done
