// ============================================================================
//  12_catch_all_and_bad_throw.cpp  —  catch(...) + KYA throw karna (aur kya nahi)
// ----------------------------------------------------------------------------
//  Build: g++ -std=c++17 -Wall -Wextra "C++ Code/12_catch_all_and_bad_throw.cpp" -o bin/12_catch_all_and_bad_throw
//
//  Do baatein: (1) `catch(...)` jo SAB kuch pakad leta hai, aur (2) C++ me tum
//  KISI BHI cheez ko throw kar sakte ho (int, string, pointer) — par karna NAHI chahiye.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠ BAD PRACTICE: `throw 42;` ya `throw "text";`                          │
//  │                                                                          │
//  │  C++ technically kisi bhi type ko throw karne deta hai. PAR:              │
//  │    - int/string/pointer `std::exception` se inherit NAHI karte            │
//  │    - to `catch(const exception&)` unhe MISS kar dega                      │
//  │    - unke paas `.what()` nahi hota (error message ka standard tareeka gaya)│
//  │  ⭐ HAMESHA `std::exception` (ya uske child) se bana object throw karo —   │
//  │     runtime_error, logic_error, ya apna custom (jo inse inherit kare).    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `catch(...)` — "sab kuch pakdo" (ellipsis catch)                      │
//  │                                                                          │
//  │  Ye HAR type ki thrown cheez ko pakad leta hai (int, string, koi bhi      │
//  │  exception). Fayda: "kuch bhi ho, program crash na ho" wala aakhri safety │
//  │  net. Nuksan: type/message pata nahi chalta (`ex.what()` nahi kar sakte). │
//  │  Isliye ise SABSE AAKHRI catch banao (specific catches ke baad), last     │
//  │  resort ki tarah.                                                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 UNCAUGHT exception: agar throw hui aur poore call stack me koi matching
//     catch nahi mila -> runtime `std::terminate()` call karta hai -> program
//     ABORT (crash). Isliye important errors ke liye kahin na kahin catch rakho.
// ============================================================================
#include <iostream>
#include <stdexcept>

using namespace std;

// ⚠ BAD: int throw kar raha hai (std::exception se nahi bana).
void throwInt() { throw 42; }

// ⚠ BAD: C-string (const char*) throw kar raha hai.
void throwUnknown() { throw "literal C-string"; }

int main() {
    cout << "=== 12 catch(...) & bad throws ===\n\n";

    cout << "--- catch(...) swallows everything ---\n";
    try {
        throwInt();
    }
    // ⭐ Ye catch MISS karega — 42 ek int hai, std::exception nahi.
    catch (const exception &) {
        cout << "Never prints for throw 42 (int does not inherit std::exception)\n";
    }
    // catch(...) — yahan girega (kyunki ye SAB kuch pakadta hai). Par `.what()` nahi mil sakta.
    catch (...) {
        cout << "catch(...) got unknown type (int 42 successfully caught)\n";
    }

    try {
        throwUnknown();
    }
    catch (...) { // C-string bhi sirf catch(...) hi pakdega
        cout << "catch(...) got C-string throw successfully\n";
    }

    cout << "\n--- Always catch const exception& when possible ---\n";
    try {
        // ✅ GOOD: std::exception family ka object throw kiya -> .what() milega,
        //    aur base catch ise pakad lega.
        throw runtime_error("good practice: inherits std::exception hierarchy");
    }
    catch (const exception &ex) {
        cout << "Proper catch: " << ex.what() << "\n";
    }

    cout << "\n--- Uncaught exception -> std::terminate (Abort) ---\n";
    cout << "Agar exception throw hui aur call path me koi matching catch nahi mila,\n";
    cout << "to runtime std::terminate() call karta hai aur process abort ho jata hai.\n";

    return 0;
}

// Expected output:
//   catch(...) got unknown type (int 42 successfully caught)
//   catch(...) got C-string throw successfully
//   Proper catch: good practice: inherits std::exception hierarchy
