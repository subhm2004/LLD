#include <bits/stdc++.h>
using namespace std;

// Demo 6: noexcept — function exception throw nahi karega (contract)

void mightThrow() { throw runtime_error("boom"); }

void willNotThrow() noexcept {
    cout << "willNotThrow() is noexcept — should not throw\n";
}

// LSP note (see L6 ExceptionRule.cpp): base/derived exception specification

class Base {
public:
    virtual void work() { cout << "Base::work\n"; }
};

class SafeDerived : public Base {
public:
    void work() noexcept override {
        cout << "SafeDerived::work (noexcept)\n";
    }
};

int main() {
    cout << "=== 06 noexcept ===\n";

    willNotThrow();

    SafeDerived d;
    Base *bp = &d;
    bp->work();

    cout << "\nIf mightThrow() called without try/catch → std::terminate (demo skipped)\n";
    cout << "Interview: noexcept on destructors default in C++11+\n";

    return 0;
}
