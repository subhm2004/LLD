#!/usr/bin/env bash
# Builds all standalone system LLD projects (C++17).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
"$ROOT/scripts/apply_cpp17_to_projects.sh" >/dev/null 2>&1 || true

declare -a PROJECTS=(
  ATM_LLD Car_Rental_System_LLD Elevator_System_LLD JSON_Parser_LLD
  Library_Management_System_LLD Linkedin_LLD LoadBalancer_LLD Logger_LLD
  Movie_Ticket_Booking_System Parking_lot_system_LLD Rate_Limiter_LLD
  Ride_sharing_app_LLD Uber_LLD URL_Shortner_LLD vending_machine_LLD
  WhatsApp_LLD "Insta_reel_LLD/yt reel architecture" LRU_Cache_LLD
  LeetCode_LLD
  OYO_Hotel_Booking_LLD
  File_Manager_LLD
  "L24 Discount_coupon_engine_LLD"
)

ok=0
fail=0
for p in "${PROJECTS[@]}"; do
  dir="$ROOT/$p"
  if [[ ! -f "$dir/compile.sh" ]]; then
    echo "SKIP (no compile.sh): $p"
    ((fail++)) || true
    continue
  fi
  if (cd "$dir" && ./compile.sh); then
    echo "OK: $p"
    ((ok++)) || true
  else
    echo "FAIL: $p"
    ((fail++)) || true
  fi
done
echo "---"
echo "Built: $ok | Failed/skipped: $fail"
