#include <bits/stdc++.h>
using namespace std;

// Demo 6: noexcept specifier — compiler ko promise karna ki ye function exception throw nahi karega

// Ye function standard error state trigger kar sakta hai.
void mightThrow() { 
    throw runtime_error("boom"); 
}

// `noexcept` keyword compiler aur callers ko ensure karta hai ki ye function exceptions safe hai.
// Isse compiler micro-optimizations perform kar sakta hai (jaise stack unwinding metadata remove karna).
void willNotThrow() noexcept {
    cout << "willNotThrow() is noexcept — should not throw\n";
}

// Liskov Substitution Principle (LSP) exception specification rule check:
// Child class method standard methods ki exceptions boundaries badha nahi sakti, par narrow (yaani safe or `noexcept`) kar sakti hai.
class Base {
public:
    virtual void work() { 
        cout << "Base::work (Normal version, might throw)\n"; 
    }
};

class SafeDerived : public Base {
public:
    // SafeDerived ne override method ko `noexcept` mark kiya hai. Ye correct design hai
    // kyunki safe implementation parent ke non-safe contracts ko break nahi karega.
    void work() noexcept override {
        cout << "SafeDerived::work (Safe version, guarantees noexcept)\n";
    }
};

int main() {
    cout << "=== 06 noexcept ===\n";

    willNotThrow();

    SafeDerived d;
    Base *bp = &d;
    bp->work(); // Base pointer dynamic dispatch ke through derived class ki work method run karega.

    cout << "\nIf mightThrow() called from inside a noexcept context → std::terminate runs immediately (destructors call skip ho jate hain).\n";
    cout << "Interview Tip: C++11 ke baad se, destructors by default `noexcept` hote hain taaki destruction process me double exception na ho.\n";

    return 0;
}
