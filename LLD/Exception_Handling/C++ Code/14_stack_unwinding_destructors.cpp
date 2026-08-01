// ============================================================================
//  14_stack_unwinding_destructors.cpp  —  Stack unwinding ko AANKHON se dekho
// ----------------------------------------------------------------------------
//  Build: g++ -std=c++17 -Wall -Wextra "C++ Code/14_stack_unwinding_destructors.cpp" -o bin/14_stack_unwinding_destructors
//
//  "Stack unwinding" = jab exception throw hoti hai, to system matching catch
//  dhoondhne ke liye call stack ko ULTA (neeche se upar) khaali karta hai, aur
//  is safai ke dauran HAR frame ke local objects ke destructors chalata hai.
//  Yahi wo mechanism hai jo RAII cleanup (file 05) ko kaam karne deta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ IS DEMO ME CALL CHAIN: main -> level1 -> level2 -> level3 (throw!)    │
//  │                                                                          │
//  │  Har level apna ek Trace object banata hai (ctor print). level3 me throw  │
//  │  hota hai. Ab dekho destructors kis ORDER me chalte hain:                 │
//  │                                                                          │
//  │     throw (level3) ─┐                                                     │
//  │                     ▼  stack UNWIND (neeche se upar):                     │
//  │     dtor level3  (jahan throw hua, wahi pehle)                            │
//  │     dtor level2                                                           │
//  │     dtor level1                                                           │
//  │     ─────────────────► phir main ka matching catch chalta hai            │
//  │                                                                          │
//  │  ⭐ ORDER = REVERSE of construction (LIFO — jo aakhri me bana wo pehle    │
//  │     mara). Ye "stack" ka natural behavior hai.                           │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 SEEKH: throw aur catch ke BEECH ke saare local objects surakshit clean ho
//     jaate hain — koi resource leak nahi (agar wo RAII objects hain). Isi liye
//     C++ me error handling + RAII = automatic, guaranteed cleanup.
//
//  ⚠ ISI LIYE destructor kabhi throw na kare: unwinding ke DAURAN (jab ek exception
//     pehle se chal rahi hai) agar destructor doosri exception phenke, to do
//     exceptions ek saath -> seedha std::terminate (file 06 wala point).
// ============================================================================
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;

// Ctor aur dtor dono print karta hai — taaki construction/destruction order dikhe.
class Trace {
public:
    explicit Trace(string label) : label_(std::move(label)) {
        cout << "  ctor  " << label_ << "\n";
    }

    ~Trace() { // stack unwinding ke dauran (aur normal exit pe) chalta hai
        cout << "  dtor  " << label_ << "\n";
    }

private:
    string label_;
};

// Sabse gehra level — yahin exception throw hoti hai.
void level3() {
    Trace t3("level3"); // banega... aur throw ke baad iska dtor SABSE pehle chalega
    throw runtime_error("boom in level3");
    // (yahan ke baad kuch nahi chalega — throw ne flow tod diya)
}

void level2() {
    Trace t2("level2");
    level3(); // yahan se exception upar aayegi -> t2 ka dtor chalega
}

void level1() {
    Trace t1("level1");
    level2(); // exception upar -> t1 ka dtor chalega
}

int main() {
    cout << "=== 14 Stack unwinding (dtors on the way up) ===\n\n";

    // level3 me throw -> stack neeche se upar unwind -> t3, t2, t1 ke dtors chalte
    // -> phir yahan (main) ka matching catch chalta hai.
    try {
        level1();
    }
    catch (const exception &ex) {
        cout << "caught at main: " << ex.what() << "\n";
    }

    cout << "\nOrder: dtor level3 -> level2 -> level1 -> phir main ka catch handler.\n";
    return 0;
}

// Expected output:
//   ctor  level1
//   ctor  level2
//   ctor  level3
//   dtor  level3   <- unwinding shuru (reverse order)
//   dtor  level2
//   dtor  level1
//   caught at main: boom in level3
