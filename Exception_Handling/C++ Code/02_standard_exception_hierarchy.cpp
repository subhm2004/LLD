// ============================================================================
//  02_standard_exception_hierarchy.cpp  —  std::exception ka parivaar (tree)
// ----------------------------------------------------------------------------
//  Build: g++ -std=c++17 -Wall -Wextra "C++ Code/02_standard_exception_hierarchy.cpp" -o bin/02_standard_exception_hierarchy
//
//  C++ ke saare standard exceptions ek COMMON base class `std::exception` se
//  inherit karte hain. Iska bada fayda: ek hi catch (base ka) saare children
//  ko pakad leta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ HIERARCHY (simplified) — do bade "khaandaan"                          │
//  │                                                                          │
//  │   std::exception                     (sabka baap)                        │
//  │     ├── logic_error   -> galtiyan jo RUN se PEHLE pakdi ja sakti thi      │
//  │     │     ├── invalid_argument   (galat argument diya)                    │
//  │     │     ├── out_of_range       (index/size limit paar)                  │
//  │     │     └── length_error                                                │
//  │     └── runtime_error -> galtiyan jo RUNTIME pe hi pata chalti hain       │
//  │           ├── range_error                                                 │
//  │           ├── overflow_error                                              │
//  │           └── underflow_error                                             │
//  │                                                                          │
//  │   logic_error  = "programmer ki galti" (code theek karo)                 │
//  │   runtime_error = "duniya ki galti" (file missing, network down...)      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ POLYMORPHIC CATCHING — is file ki asli seekh:
//     `catch (const std::exception &ex)` LIKHO, to logic_error, out_of_range,
//     runtime_error — SAB ismein aa jaate hain (kyunki sab uske children hain).
//     Ek jagah, sab handle. (Base reference derived ko pakadti hai.)
//
//  📌 `ex.what()` har exception pe milta hai (base me virtual hai) — isi liye
//     base reference se bhi sahi (derived ka) message milta hai.
// ============================================================================
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

// Teen alag-alag standard exceptions throw karne wale helpers:
void throwLogicError() {
    // logic_error: "logic me gadbad" — theory me isse code review me pakda ja sakta.
    throw logic_error("logic_error from std");
}

void throwOutOfRange() {
    // out_of_range: index/bounds paar (jaise vector[100] jab size 10 ho).
    // Ye logic_error ka CHILD hai.
    throw out_of_range("out_of_range from vector-like API");
}

void throwRuntime() {
    // runtime_error: aisi galti jo compile-time pe predict nahi hoti (runtime pe aati).
    throw runtime_error("runtime_error — common in LLD repos");
}

int main() {
    cout << "=== 02 Standard exception hierarchy ===\n\n";

    // Ek chhota helper: label print karo, function chalao, aur agar wo throw kare
    // to BASE reference se catch karo.
    auto demo = [](const string &label, function<void()> fn) {
        cout << label << " -> ";
        try {
            fn();
        }
        // ⭐ YAHI POINT HAI: base `std::exception` se catch — teeno alag children
        //    (logic_error / out_of_range / runtime_error) isi ek block me match ho jaate.
        catch (const exception &ex) {
            cout << "caught exception: " << ex.what() << "\n";
        }
    };

    demo("logic_error", throwLogicError);
    demo("out_of_range", throwOutOfRange);
    demo("runtime_error", throwRuntime);

    cout << "\nHierarchy (simplified):\n";
    cout << "  exception (Base Class)\n";
    cout << "    |-- logic_error (Errors checkable before runtime) -> invalid_argument, out_of_range, ...\n";
    cout << "    `-- runtime_error (Errors unpredictable before runtime) -> range_error, overflow_error, ...\n";

    return 0;
}

// Expected output:
//   logic_error -> caught exception: logic_error from std
//   out_of_range -> caught exception: out_of_range from vector-like API
//   runtime_error -> caught exception: runtime_error — common in LLD repos
