// C++17 — Stack Unwinding: Exception propagation ke dauran call stack ke local objects ke destructors running behavior demo
#include <bits/stdc++.h>
using namespace std;

/**
 * @class Trace
 * @brief Scope creation aur destruction trace karne ke liye simple utility class.
 */
class Trace {
public:
    explicit Trace(string label) : label_(std::move(label)) {
        cout << "  ctor  " << label_ << "\n";
    }
    
    // Destructor: Stack clear hone par automatic run karta hai.
    ~Trace() { 
        cout << "  dtor  " << label_ << "\n"; 
    }

private:
    string label_;
};

// Sub-call level 3: Exception initiate karega.
void level3() {
    Trace t3("level3");
    throw runtime_error("boom in level3"); // yahan exception throw hui!
}

// Sub-call level 2: level3 call karega.
void level2() {
    Trace t2("level2");
    level3();
}

// Sub-call level 1: level2 call karega.
void level1() {
    Trace t1("level1");
    level2();
}

int main() {
    cout << "=== 14 Stack unwinding (dtors on the way up) ===\n\n";

    // Stack Unwinding Rule: Jab level3 me exception throw hoti hai, toh run-time system matching catch block ki khoj me active stack frames ko reverse order me crawl/unwind karta hai.
    // Is reverse crawl ke dauran, har level ke stack pointer se associated local objects (jaise level3, level2, level1) ke destructors bottom-to-top sequence me call hote hain.
    // Yeh guaranteed resource cleanup support (RAII) C++ error system ka core model hai.
    try {
        level1();
    } 
    catch (const exception &ex) {
        cout << "caught at main: " << ex.what() << "\n";
    }

    cout << "\nOrder details: first dtor level3 -> then level2 -> then level1 -> finally matching catch handler runs.\n";
    return 0;
}
