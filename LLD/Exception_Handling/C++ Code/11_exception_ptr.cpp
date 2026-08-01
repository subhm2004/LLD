// ============================================================================
//  11_exception_ptr.cpp  —  Exception ko "pakdo, store karo, baad me phenko"
// ----------------------------------------------------------------------------
//  Build: g++ -std=c++17 -Wall -Wextra "C++ Code/11_exception_ptr.cpp" -o bin/11_exception_ptr
//
//  Normally exception turant catch hoti hai. Par kabhi hume exception ko ek jagah
//  PAKAD ke, ek VARIABLE me RAKH ke, KAHIN AUR (doosra thread / baad me) handle
//  karna hota hai. Iske liye C++ deta hai `std::exception_ptr`.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SABSE BADA USE-CASE: MULTI-THREADING                                  │
//  │                                                                          │
//  │  Worker thread me exception aa gayi — par exceptions THREAD BOUNDARY paar │
//  │  nahi kar sakti (ek thread ki exception doosre me seedhe nahi jaati).     │
//  │  Solution: worker exception ko `exception_ptr` me capture kare, wo ptr    │
//  │  main thread ko de, main thread use rethrow karke handle kare.           │
//  │  (std::async/std::future andar isi mechanism se kaam karte hain.)        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ TEEN KEY FUNCTIONS:
//     std::current_exception()  -> catch(...) ke andar, "abhi wali" exception ko
//                                  ek exception_ptr me pakad lo (nullptr agar koi nahi)
//     std::rethrow_exception(ep)-> stored exception ko WAPAS active throw karo
//                                  (taaki normal catch use pakad sake)
//     exception_ptr             -> ek shared, copyable "handle" us exception ka
//
//  📌 exception_ptr null bhi ho sakta hai (koi error na aaya to) — `if (!ep)`
//     se check karo.
// ============================================================================
#include <exception>
#include <iostream>
#include <stdexcept>

using namespace std;

// Ek task (jaise worker thread ka kaam). Fail hua to exception ko CAPTURE karke
// exception_ptr lauta deta hai (throw nahi karta — caller ko safely transfer).
exception_ptr runTask(bool fail) {
    exception_ptr ep = nullptr; // khaali se shuru (koi error nahi)
    try {
        if (fail) {
            throw runtime_error("Task failed in asynchronous worker thread context.");
        }
        cout << "Task completed normally without errors.\n";
    }
    // ⭐ catch(...) — har type pakadta hai. Andar current_exception() se "abhi wali"
    //    exception ko exception_ptr me store kar lete hain (bina handle kiye).
    catch (...) {
        ep = current_exception(); // exception ab ek variable me "packed" hai
    }
    return ep; // caller ko de do — wo jab chahe rethrow karke handle kare
}

// Stored exception ko log karke aage badha do.
void logAndRethrow(const exception_ptr &ep) {
    if (!ep) {
        return; // null = koi error hi nahi tha
    }
    try {
        // ⭐ rethrow_exception -> packed exception ko WAPAS active throw karo,
        //    taaki neeche wala normal catch(const exception&) use pakad sake.
        rethrow_exception(ep);
    }
    catch (const exception &ex) {
        cout << "Logged in main thread (Coordinator layer): " << ex.what() << "\n";
        throw; // optional: aur upar bhejo
    }
}

int main() {
    cout << "=== 11 std::exception_ptr ===\n";

    // Failure case: task fail -> exception capture -> alag jagah handle
    exception_ptr err = runTask(true);
    try {
        logAndRethrow(err);
    }
    catch (const exception &ex) {
        cout << "Handled after rethrow in outer block: " << ex.what() << "\n";
    }

    // Success case: koi error nahi -> exception_ptr null
    cout << "\nSuccess path execution:\n";
    exception_ptr ok = runTask(false);
    cout << (ok ? "Stored error found.\n" : "No error stored (nullptr pointer).\n");

    return 0;
}

// Expected output:
//   Logged in main thread (Coordinator layer): Task failed ...
//   Handled after rethrow in outer block: Task failed ...
//   Success path execution:
//   Task completed normally without errors.
//   No error stored (nullptr pointer).
