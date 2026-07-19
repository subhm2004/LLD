// C++17 — nested try + "finally" execution blocks via RAII (C++ does not have a native 'finally' keyword)
#include <bits/stdc++.h>
using namespace std;

/**
 * @class ScopeGuard
 * @brief Java/Python ke 'finally' keyword ki tarah cleanups implement karne ka RAII class pattern.
 * 
 * C++ me `finally` block nahi hota. Par scope jab delete hota hai, tab stack destructors run karte hain.
 * Hum ek generic `ScopeGuard` class banate hain jiska destructor scope end hone par execute ho, 
 * chahe exception aayi ho ya transaction normal finish hua ho (Scope Guard Pattern).
 */
class ScopeGuard {
public:
    explicit ScopeGuard(function<void()> onExit) : onExit_(std::move(onExit)) {}
    
    // Destructor scope exit hone par lambda function run kar deta hai (Guaranteed finally-like behavior).
    ~ScopeGuard() {
        if (onExit_) {
            onExit_();
        }
    }

    // Double execution se bachne ke liye copy operations disable karte hain.
    ScopeGuard(const ScopeGuard &) = delete;
    ScopeGuard &operator=(const ScopeGuard &) = delete;

private:
    function<void()> onExit_;
};

/**
 * @brief Demo function jo nested try-catch blocks aur ScopeGuard cleanups dikhata hai.
 * 
 * @param failPayment bool flag jo payment gateway response simulate karta hai.
 */
void processOrder(bool failPayment) {
    cout << "  [finally-style] open audit log\n";
    
    // Guard construct kiya. Scope (function stack frame) exit hote hi iska lambda automatic chalega.
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
    // Inner catch block: Local exceptions catch karta hai taaki local rollback perform kiya ja sake.
    catch (const runtime_error &ex) {
        cout << "  inner catch: " << ex.what() << " — rollback inventory\n";
        throw; // upper block ko flow notify karne ke liye rethrow.
    }
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
