// C++17 — nested try + "finally" via RAII (C++ has no finally keyword)
#include <bits/stdc++.h>
using namespace std;

class ScopeGuard {
public:
    explicit ScopeGuard(function<void()> onExit) : onExit_(std::move(onExit)) {}
    ~ScopeGuard() {
        if (onExit_) {
            onExit_();
        }
    }

    ScopeGuard(const ScopeGuard &) = delete;
    ScopeGuard &operator=(const ScopeGuard &) = delete;

private:
    function<void()> onExit_;
};

void processOrder(bool failPayment) {
    cout << "  [finally-style] open audit log\n";
    ScopeGuard guard([] { cout << "  [finally-style] close audit log (always)\n"; });

    try {
        cout << "  validate cart OK\n";
        if (failPayment) {
            throw runtime_error("payment gateway timeout");
        }
        cout << "  payment captured\n";
    } catch (const runtime_error &ex) {
        cout << "  inner catch: " << ex.what() << " — rollback inventory\n";
        throw; // propagate to outer
    }
}

int main() {
    cout << "=== 10 Nested try + finally via RAII ===\n\n";

    cout << "Case A: success\n";
    try {
        processOrder(false);
        cout << "outer: order completed\n";
    } catch (const exception &ex) {
        cout << "outer catch: " << ex.what() << "\n";
    }

    cout << "\nCase B: failure (audit still closes)\n";
    try {
        processOrder(true);
    } catch (const exception &ex) {
        cout << "outer catch: " << ex.what() << "\n";
    }

    return 0;
}
