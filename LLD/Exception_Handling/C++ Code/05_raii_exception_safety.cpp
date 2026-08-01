// ============================================================================
//  05_raii_exception_safety.cpp  —  RAII: exception aane par bhi cleanup pakka
// ----------------------------------------------------------------------------
//  Build: g++ -std=c++17 -Wall -Wextra "C++ Code/05_raii_exception_safety.cpp" -o bin/05_raii_exception_safety
//
//  RAII = "Resource Acquisition Is Initialization". Fancy naam, simple idea:
//     - resource LO constructor me (file open, memory alloc, lock)
//     - resource CHHODO destructor me (file close, delete, unlock)
//  Aur destructor scope khatam hote hi APNE AAP chalta hai — chahe normal flow ho
//  ya EXCEPTION aa jaye. Isi liye RAII exception-safe cleanup ka C++ tareeka hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ EXCEPTION aaye tab bhi DESTRUCTOR chalta hai — yahi jaadu hai         │
//  │                                                                          │
//  │  Jab `throw` hota hai, "stack unwinding" shuru hoti hai: system current   │
//  │  function ke saare LOCAL objects ke destructors call karta hai (aage badhne│
//  │  se pehle). To agar tumne resource ek stack object (RAII) me wrap kiya hai,│
//  │  wo GUARANTEED release hoga — chahe beech me exception aa jaaye.          │
//  │                                                                          │
//  │  ⚠ ISI LIYE raw `new`/`delete` KHATARNAK hai: agar `new` ke baad aur      │
//  │     `delete` se pehle exception aa gayi, to `delete` chalta hi nahi ->    │
//  │     MEMORY LEAK. RAII (unique_ptr/stack object) me ye problem hi nahi.    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Ye poori file "throw ke baad bhi Resource released kyun dikhta hai" prove
//     karti hai — dono case me (stack object aur unique_ptr).
// ============================================================================
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

using namespace std;

// Ek resource (jaise DB connection) — RAII rule: acquire ctor me, release dtor me.
class Resource {
public:
    explicit Resource(string name) : name_(std::move(name)) {
        cout << "  [Resource] acquired: " << name_ << "\n"; // <- acquire
    }

    // ⭐ Ye destructor scope khatam hote hi chalega — normal ya exceptional, dono me.
    ~Resource() {
        cout << "  [Resource] released: " << name_ << "\n"; // <- release (guaranteed)
    }

    void use() const { cout << "  [Resource] using " << name_ << "\n"; }

private:
    string name_;
};

// fail == true -> resource acquire karne ke BAAD exception throw karega.
void mayThrow(bool fail) {
    Resource r("DB connection"); // stack object (RAII)
    r.use();

    if (fail) {
        // ⭐ Yahan throw hua -> stack unwinding -> `r` ka destructor AUTOMATICALLY
        //    chalega (Resource released print hoga) — phir exception upar jaayegi.
        throw runtime_error("operation failed after resource acquired");
    }

    cout << "  Operation completed normally\n";
}

int main() {
    cout << "=== 05 RAII & exception safety ===\n\n";

    cout << "Case A: exception after Resource on stack\n";
    try {
        mayThrow(true);
    } catch (const exception &ex) {
        cout << "Caught: " << ex.what() << "\n";
        cout << "(Notice: Resource destructor still ran safely due to stack unwinding)\n\n";
    }

    cout << "Case B: unique_ptr — same idea for heap\n";
    try {
        // ⭐ `unique_ptr` = RAII for heap. Andar `new` hai, par uska `delete`
        //    destructor me hai. Exception aaye to bhi unique_ptr apne aap delete
        //    karega -> NO LEAK (raw pointer hota to leak ho jaata).
        auto ptr = make_unique<Resource>("smart pointer");
        ptr->use();
        throw runtime_error("fail after unique_ptr");
    } catch (const exception &ex) {
        cout << "Caught: " << ex.what() << "\n";
    }

    cout << "\nRule: LLD me memory leak se bachne ke liye hamesha unique_ptr / stack objects (RAII) use karo, raw new/delete nahi.\n";
    return 0;
}

// Expected output (dhyaan do "released" throw ke baad bhi chalta hai):
//   Case A: ... acquired -> using -> released -> Caught: ...
//   Case B: ... acquired -> using -> released -> Caught: ...
