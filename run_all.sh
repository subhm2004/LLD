#!/usr/bin/env bash
# ============================================================================
#  run_all.sh — saare runnable demos build karo aur CHALAO
# ----------------------------------------------------------------------------
#  Ye repo ka REGRESSION TEST hai.
#
#  Har demo ke andar `demo::check(...)` se asli invariants verify hote hain
#  (jaise "Bloom filter me false negative kabhi nahi", "koi policy Belady
#  optimal se upar nahi ja sakti"). Agar koi invariant toota to wo demo
#  EXIT CODE 1 deta hai, aur ye script fail ho jaati hai.
#
#  Yaani sirf "compile ho gaya" kaafi nahi — algorithms ka BEHAVIOUR bhi
#  check hota hai.
#
#  Chalao:
#     ./run_all.sh          # sab build + run (CI yahi chalata hai)
#     ./run_all.sh build    # sirf build karo, chalao mat
#
# ============================================================================
set -uo pipefail

cd "$(dirname "$0")"

CXX="${CXX:-g++}"
FLAGS=(-std=c++17 -Wall -Wextra -O2 -pthread)

MODE="${1:-all}"

# Har entry: "folder|include-dir|sources..."
# (include-dir alag isliye ki kuch demos subfolder me hain)
FOLDERS=(
    "HLD/Consistent_Hashing"
    "HLD/Load_Balancer_Algorithms"
    "HLD/Rate_Limiting_Algorithms"
    "HLD/Probabilistic_Data_Structures"
    "HLD/Cache_Eviction_Policies"
    "LLD/Consistent_Hashing_LLD"
)

BUILD_DIR="$(mktemp -d)"
trap 'rm -rf "$BUILD_DIR"' EXIT

# Ek demo ko kitne second se zyada nahi chalne dena
DEMO_TIMEOUT="${DEMO_TIMEOUT:-120}"

# ---- Portable timeout (macOS pe `timeout` command hoti hi nahi) ------------
#  ⚠ Ye zaroori kyun hai: kuch bug INFINITE LOOP bana dete hain. Jaise CLOCK
#     cache me agar "second chance" wala bit clear karna bhool jao, to victim
#     dhoondhne wali sui HAMESHA ghoomti rehti hai aur kabhi rukti nahi.
#     Bina timeout ke aisa demo CI ko latka deta (fail nahi hota, bas atak
#     jaata). Timeout usse ek SAAF failure bana deta hai.
run_with_timeout() {
    local seconds="$1"
    shift

    "$@" &
    local pid=$!

    ( sleep "$seconds"; kill -9 "$pid" 2>/dev/null ) &
    local watcher=$!

    wait "$pid" 2>/dev/null
    local status=$?

    kill -9 "$watcher" 2>/dev/null
    wait "$watcher" 2>/dev/null
    return $status
}

total=0
failed=0
declare -a FAILURES=()

echo "==========================================================="
echo " System Design repo — saare demos build + verify"
echo "==========================================================="
echo

for folder in "${FOLDERS[@]}"; do
    echo "--- $folder"

    # Us folder ke saare demo sources (main.cpp bhi shaamil)
    while IFS= read -r src; do
        [ -z "$src" ] && continue

        name="$(echo "$src" | tr '/' '_' | sed 's/\.cpp$//')"
        binary="$BUILD_DIR/$name"

        total=$((total + 1))

        # Include root = folder khud (demos `"../lb_common.h"` jaisa use karte hain)
        if ! "$CXX" "${FLAGS[@]}" -I "$folder" "$src" -o "$binary" 2>"$BUILD_DIR/$name.err"; then
            echo "    ❌ BUILD FAIL : $src"
            sed 's/^/        /' "$BUILD_DIR/$name.err" | head -15
            failed=$((failed + 1))
            FAILURES+=("BUILD  $src")
            continue
        fi

        if [ "$MODE" = "build" ]; then
            echo "    ✅ built      : $(basename "$src")"
            continue
        fi

        # ⭐ Yahan asli baat hai: demo ka EXIT CODE dekhte hain.
        #    Invariant toota to demo 1 return karta hai.
        if run_with_timeout "$DEMO_TIMEOUT" "$binary" >"$BUILD_DIR/$name.out" 2>&1; then
            echo "    ✅ pass       : $(basename "$src")"
        else
            status=$?
            if [ "$status" -ge 128 ]; then
                echo "    ❌ TIMEOUT/HANG: $src  (${DEMO_TIMEOUT}s se zyada laga — infinite loop?)"
                FAILURES+=("TIMEOUT $src")
            else
                echo "    ❌ VERIFY FAIL: $src"
                grep -E "VERIFY FAIL|VERIFY:" "$BUILD_DIR/$name.out" | sed 's/^/        /' | head -10
                FAILURES+=("VERIFY $src")
            fi
            failed=$((failed + 1))
        fi
    done < <(find "$folder" -name "*.cpp" | sort)
    echo
done

echo "==========================================================="
if [ "$failed" -eq 0 ]; then
    echo " ✅ SAB THEEK — $total demos build hue aur verify pass kiya"
    echo "==========================================================="
    exit 0
fi

echo " ❌ $failed / $total demos FAIL hue:"
for f in "${FAILURES[@]}"; do
    echo "      $f"
done
echo "==========================================================="
exit 1
