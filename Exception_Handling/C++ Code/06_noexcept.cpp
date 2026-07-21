// ============================================================================
//  06_noexcept.cpp  —  `noexcept`: "ye function kabhi throw nahi karega" ka vaada
// ----------------------------------------------------------------------------
//  Build: g++ -std=c++17 -Wall -Wextra "C++ Code/06_noexcept.cpp" -o bin/06_noexcept
//
//  `noexcept` ek promise hai jo function apne callers aur compiler se karta hai:
//  "main koi exception bahar nahi phenkunga."
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ noexcept ka FAYDA — compiler optimization + clarity                   │
//  │                                                                          │
//  │  Agar compiler jaanta hai ki function throw nahi karega, to use "stack    │
//  │  unwinding ka intezaam" (metadata/bookkeeping) rakhne ki zaroorat nahi -> │
//  │  thoda tez/chhota code. Aur reader ko turant pata: "ye safe hai".        │
//  │                                                                          │
//  │  ⚠ WAADA TODA to SAZA badi: agar `noexcept` function ke andar se koi      │
//  │  exception bahar nikal AAYI, to program `std::terminate()` call karke     │
//  │  TURANT MAR jaata hai (koi catch nahi, koi cleanup nahi). Isliye noexcept │
//  │  sirf tab lagao jab SACH me pakka ho ki throw nahi hoga.                 │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ MOVE operations pe noexcept KYUN important hai (bada interview point):
//     std::vector jab grow hoti hai to elements ko naye buffer me shift karti hai.
//     Agar move-constructor `noexcept` hai, vector MOVE karti hai (fast). Nahi to
//     safety ke liye COPY karti hai (slow) — kyunki move ke beech throw ho jaaye
//     to vector aadhi-adhoori reh jaati. Isliye move ctor/assignment ko noexcept banao.
//
//  📌 DESTRUCTORS C++11 se by-default `noexcept` hote hain — taaki stack unwinding
//     (jo pehle se ek exception handle kar rahi hai) ke beech doosri exception na
//     aaye. Do exceptions ek saath = seedha terminate. Isliye destructor kabhi throw na kare.
// ============================================================================
#include <iostream>
#include <stdexcept>

using namespace std;

// Ye function throw kar sakta hai (noexcept NAHI hai).
void mightThrow() { throw runtime_error("boom"); }

// ⭐ `noexcept` -> ye kabhi throw nahi karega (aur compiler ispe bharosa karega).
void willNotThrow() noexcept {
    cout << "willNotThrow() is noexcept — should not throw\n";
}

// ---- LSP + noexcept: override method exception guarantee ko TIGHT kar sakta hai --
class Base {
public:
    virtual void work() { // base "throw kar sakta hoon" (loose guarantee)
        cout << "Base::work (Normal version, might throw)\n";
    }
    virtual ~Base() = default;
};

class SafeDerived : public Base {
public:
    // ⭐ Override ne guarantee ko TIGHT (safe) kar diya (noexcept). Ye ALLOWED hai:
    //    parent "throw kar sakta hoon" bolta tha, child "main to throw karunga hi
    //    nahi" bolta hai — ye parent ke contract ko TODTA nahi (aur safe banata).
    //    (Ulta galat hota: parent noexcept, child throw kare -> contract break.)
    void work() noexcept override {
        cout << "SafeDerived::work (Safe version, guarantees noexcept)\n";
    }
};

int main() {
    cout << "=== 06 noexcept ===\n";

    willNotThrow();

    SafeDerived d;
    Base *bp = &d;
    bp->work(); // dynamic dispatch -> SafeDerived::work chalega

    cout << "\nIf mightThrow() called from inside a noexcept context -> std::terminate runs immediately (destructors skip).\n";
    cout << "Tip: C++11 ke baad destructors by default noexcept hote hain (double-exception se bachne ke liye).\n";

    // Note: mightThrow() ko jaan-boojh ke yahan call NAHI kiya (wo demo ke liye hai).
    return 0;
}

// Expected output:
//   willNotThrow() is noexcept — should not throw
//   SafeDerived::work (Safe version, guarantees noexcept)
