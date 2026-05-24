#include <iostream>
using namespace std;

// Base: teen access levels
class Base {
public:
    int pub = 1;
protected:
    int prot = 2;
private:
    int priv = 3;
public:
    void showBase() const {
        cout << "  Base sees: pub=" << pub << " prot=" << prot << " priv=" << priv << endl;
    }
};

// --- public inheritance (default for IS-A in interviews) ---
class PublicChild : public Base {
public:
    void showChild() const {
        cout << "[public inheritance] Child can use: pub=" << pub << " prot=" << prot;
        // priv — compile error if uncommented
        cout << endl;
    }
};

class ProtectedChild : protected Base {
public:
    void showChild() const {
        cout << "[protected inheritance] inside child: pub=" << pub << " prot=" << prot << endl;
    }
};

class PrivateChild : private Base {
public:
    void showChild() const {
        cout << "[private inheritance] inside child: pub=" << pub << " prot=" << prot << endl;
    }
    // Expose one base method so main can demo
    using Base::showBase;
};

void tryOutside(Base& b) { (void)b; /* pub OK via reference if public inheritance path */ }

int main() {
    cout << "=== Access in Base ===\n";
    Base b;
    b.showBase();

    cout << "\n=== public : public Base ===\n";
    PublicChild pc;
    pc.pub = 10;       // still public
    pc.showChild();
    pc.showBase();
    Base* up = &pc;    // OK — IS-A for outsiders
    (void)up;

    cout << "\n=== protected : protected Base ===\n";
    ProtectedChild prc;
    prc.showChild();
    // prc.pub = 5;    // ERROR: pub became protected
    // Base* bad = &prc; // ERROR: not IS-A publicly

    cout << "\n=== private : private Base ===\n";
    PrivateChild pric;
    pric.showChild();
    pric.showBase();
    // pric.pub — ERROR

    cout << "\n[Summary] public inheritance: outside world sees same access as Base.\n";
    cout << "protected/private inheritance: tighten access — rare in application code.\n";
    return 0;
}
