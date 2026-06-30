// ============================================================================
//  18_Conversion_Functions.cpp  —  Type conversion functions
// ----------------------------------------------------------------------------
//  C++ ek type ko doosre me convert karne ke do tareeke deta hai:
//    1) Single-argument constructor -> doosre type se IS class me convert.
//    2) Conversion operator (operator T()) -> IS class se doosre type me convert.
//  Implicit conversion convenient hai par GALAT jagah chup-chap convert ho sakta
//  hai -> isliye `explicit` keyword se implicit conversion rok sakte ho.
// ============================================================================
#include <bits/stdc++.h>
using namespace std;

// --- 1) Single-argument ctor = implicit conversion source ---
class Meters {
    double value;
public:
    Meters(double m) : value(m) {  // NOT explicit — implicit conversion allowed
        cout << "  Meters ctor from double: " << value << endl;
    }
    double get() const { return value; }
};

class MetersExplicit {
    double value;
public:
    explicit MetersExplicit(double m) : value(m) {
        cout << "  MetersExplicit ctor from double: " << value << endl;
    }
    double get() const { return value; }
};

// --- 2) Conversion operator (user-defined conversion FROM class) ---
class Fraction {
    int num, den;
public:
    Fraction(int n, int d) : num(n), den(d) {}
    // Implicit: Fraction → double
    operator double() const {
        return static_cast<double>(num) / den;
    }
};

class SafeBool {
    bool flag;
public:
    explicit SafeBool(bool f) : flag(f) {}
    // explicit conversion operator (C++11) — no if (obj) surprise
    explicit operator bool() const { return flag; }
};

void printMeters(Meters m) {
    cout << "  printMeters got " << m.get() << " m\n";
}

int main() {
    cout << "=== Implicit conversion (single-arg ctor) ===\n";
    Meters m1 = 5.5;           // same as Meters(5.5) — compiler converts
    printMeters(10);           // double → Meters implicit
    (void)static_cast<Meters>(3.3);  // explicit cast OK

    cout << "\n=== explicit ctor — blocks implicit ===\n";
    // MetersExplicit bad = 5;     // ERROR without explicit
    MetersExplicit me(7.0);
    (void)static_cast<MetersExplicit>(7.0);

    cout << "\n=== Conversion operator Fraction → double ===\n";
    Fraction half(1, 2);
    double d = half;             // implicit via operator double()
    cout << "  half as double = " << d << endl;
    cout << "  half + 0.5 = " << half + 0.5 << endl;

    cout << "\n=== explicit operator bool ===\n";
    SafeBool sb(true);
    // if (sb) { }               // ERROR: explicit operator bool
    if (static_cast<bool>(sb)) {
        cout << "  static_cast<bool>(sb) works\n";
    }
    if (sb.operator bool()) {
        cout << "  sb.operator bool() works\n";
    }

    cout << "\nRule: use explicit on single-arg ctors & conversion operators unless implicit is intended.\n";
    return 0;
}
