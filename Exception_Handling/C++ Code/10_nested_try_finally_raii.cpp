// ============================================================================
//  10_nested_try_finally_raii.cpp  —  C++ me "finally" nahi hai — RAII se banao
// ----------------------------------------------------------------------------
//  Build: g++ -std=c++17 -Wall -Wextra "C++ Code/10_nested_try_finally_raii.cpp" -o bin/10_nested_try_finally_raii
//
//  Java/Python me `try { } finally { }` hota hai — "finally" wala code HAMESHA
//  chalta hai (success ho ya exception). C++ me `finally` keyword HAI HI NAHI.
//  Par C++ ke paas isse behtar cheez hai: RAII destructors.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SCOPE GUARD PATTERN — "finally" ka C++ jugaad                        │
//  │                                                                          │
//  │  Ek chhoti class `ScopeGuard` banao jo ek lambda (cleanup code) rakhe,   │
//  │  aur uske DESTRUCTOR me wo lambda chala de. Kyunki destructor scope       │
//  │  khatam hote hi chalta hai (normal ya exception, dono me — file 05),      │
//  │  wo lambda "finally" ki tarah GUARANTEED chalega.                        │
//  │                                                                          │
//  │     ScopeGuard g([]{ close_file(); });  // ye lambda scope end pe chalega │
//  │                                                                          │
//  │  Fayda over Java-finally: cleanup "declare" wahin hota hai jahan resource │
//  │  banta hai (paas-paas), aur multiple guards apne aap ulte order me chalte.│
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ COPY DELETE zaroori: agar ScopeGuard copy ho jaaye to lambda DO baar
//     chal jaata (dono copies ke destructor). Isliye copy = delete (ek hi baar).
//
//  📌 Ye file nested try (inner rollback + rethrow) + ScopeGuard (audit log close)
//     dono ek saath dikhati hai. Notice: dono case (success/fail) me "close audit
//     log" chalta hai — kyunki wo guard ke destructor me hai.
// ============================================================================
#include <functional>
#include <iostream>
#include <stdexcept>
#include <utility>

using namespace std;

// ---- "finally" ka RAII roop ------------------------------------------------
class ScopeGuard {
public:
    explicit ScopeGuard(function<void()> onExit) : onExit_(std::move(onExit)) {}

    // ⭐ Destructor scope exit pe lambda chala deta hai (guaranteed "finally").
    ~ScopeGuard() {
        if (onExit_) {
            onExit_();
        }
    }

    // Double-run se bachne ke liye copy band (upar note).
    ScopeGuard(const ScopeGuard &) = delete;
    ScopeGuard &operator=(const ScopeGuard &) = delete;

private:
    function<void()> onExit_; // cleanup code (kya karna hai exit pe)
};

// failPayment == true -> payment step fail -> rollback + rethrow.
void processOrder(bool failPayment) {
    cout << "  [finally-style] open audit log\n";

    // ⭐ Guard bana diya. `processOrder` se nikalte hi (success ya exception),
    //    iska lambda "close audit log" chalega — GUARANTEED.
    ScopeGuard guard([] {
        cout << "  [finally-style] close audit log (always runs on exit)\n";
    });

    try {
        cout << "  validate cart OK\n";
        if (failPayment) {
            throw runtime_error("payment gateway timeout");
        }
        cout << "  payment captured\n";
    }
    // Inner catch: LOCAL rollback karo, phir upar ko batao (rethrow).
    catch (const runtime_error &ex) {
        cout << "  inner catch: " << ex.what() << " — rollback inventory\n";
        throw; // rethrow -> outer block handle karega (par guard phir bhi chalega!)
    }
    // ⭐ Notice: throw hone par bhi, function se nikalte waqt `guard` ka destructor
    //    chalega -> "close audit log" print hoga. Yahi "finally" behavior hai.
}

int main() {
    cout << "=== 10 Nested try + finally via RAII ===\n\n";

    cout << "Case A: success path\n";
    try {
        processOrder(false);
        cout << "outer: order completed successfully\n";
    } catch (const exception &ex) {
        cout << "outer catch: " << ex.what() << "\n";
    }

    cout << "\nCase B: failure path (audit log closes anyway)\n";
    try {
        processOrder(true);
    } catch (const exception &ex) {
        cout << "outer catch: " << ex.what() << "\n";
    }

    return 0;
}

// Expected output (dono case me "close audit log" chalta hai):
//   Case A: open -> validate -> payment captured -> CLOSE audit log -> completed
//   Case B: open -> validate -> inner catch rollback -> CLOSE audit log -> outer catch
