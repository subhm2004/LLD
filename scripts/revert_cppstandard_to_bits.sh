#!/usr/bin/env bash
# Reverts CppStandard.h back to #include <bits/stdc++.h> in system LLD projects.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"

declare -a PROJECTS=(
  "ATM_LLD"
  "Car_Rental_System_LLD"
  "Elevator_System_LLD"
  "JSON_Parser_LLD"
  "Library_Management_System_LLD"
  "Linkedin_LLD"
  "LoadBalancer_LLD"
  "Logger_LLD"
  "Movie_Ticket_Booking_System"
  "Parking_lot_system_LLD"
  "Rate_Limiter_LLD"
  "Ride_sharing_app_LLD"
  "Uber_LLD"
  "URL_Shortner_LLD"
  "vending_machine_LLD"
  "WhatsApp_LLD"
  "Insta_reel_LLD/yt reel architecture"
  "LRU_Cache_LLD"
  "LFU_Cache_LLD"
)

replace_in_tree() {
  local dir="$1"
  find "$dir" -type f \( -name '*.h' -o -name '*.hpp' -o -name '*.cpp' \) -print0 \
    | while IFS= read -r -d '' file; do
        perl -pi -e '
          s/#include\s+"\.\.\/config\/CppStandard\.h"/#include <bits\/stdc++.h>/g;
          s/#include\s+"config\/CppStandard\.h"/#include <bits\/stdc++.h>/g;
          s/#include\s+"\.\.\/CppStandard\.h"/#include <bits\/stdc++.h>/g;
          s/#include\s+"CppStandard\.h"/#include <bits\/stdc++.h>/g;
        ' "$file"
      done
}

for rel in "${PROJECTS[@]}"; do
  dir="$ROOT/$rel"
  [[ -d "$dir" ]] || continue
  echo "Reverting: $rel"
  replace_in_tree "$dir"
  rm -f "$dir/CppStandard.h"
  rm -f "$dir/config/CppStandard.h"
done

echo "Done."
