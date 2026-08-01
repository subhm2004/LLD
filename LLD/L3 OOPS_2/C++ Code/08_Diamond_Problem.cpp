// ============================================================================
//  08_Diamond_Problem.cpp  —  Diamond problem & virtual inheritance
// ----------------------------------------------------------------------------
//  Diamond: A -> B, A -> C, aur D : B, C. Ab D ke paas A ki DO copies aa jaati
//  hain -> ambiguity (kaunsi A?). Fix: B aur C, A ko `virtual` inherit karein
//  (virtual base class) -> tab D me A ki sirf EK shared copy rehti hai.
//  Yahi reason hai multiple inheritance ko soch-samajh ke use karte hain.
// ============================================================================
#include <iostream>
using namespace std;

// --- WITHOUT virtual inheritance (problem) ---
class A {
public:
    int value = 1;
    void show() { cout << "A::show value=" << value << "\n"; }
};

class B : public A {};
class C : public A {};
class D_bad : public B, public C {
public:
    void demo() {
        // B::value = 2;  // ambiguous: B::A or C::A?
        // show();        // ambiguous: two A subobjects
        cout << "D_bad has TWO A subobjects → ambiguity\n";
    }
};

// --- WITH virtual inheritance (fix) ---
class A_virt {
public:
    int value = 1;
    virtual void show() { cout << "A_virt::show value=" << value << "\n"; }
    virtual ~A_virt() = default;
};

class Bv : public virtual A_virt {};
class Cv : public virtual A_virt {};
class D_good : public Bv, public Cv {
public:
    void show() override {
        value = 99;
        cout << "D_good single A_virt subobject, value=" << value << "\n";
    }
};

int main() {
    cout << "sizeof(D_bad) > sizeof(single A) — duplicate base\n";
    D_bad db;
    db.demo();

    cout << "\n=== Virtual inheritance fix ===\n";
    D_good dg;
    dg.show();
    cout << "sizeof(D_good): " << sizeof(D_good) << " (one shared A_virt)\n";
    return 0;
}
