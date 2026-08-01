// ============================================================================
//  04_catch_order_and_rethrow.cpp  —  Catch ka sahi ORDER + REthrow
// ----------------------------------------------------------------------------
//  Build: g++ -std=c++17 "C++ Code/04_catch_order_and_rethrow.cpp" -o bin/04_catch_order_and_rethrow
//
//  Do important cheezein:
//    1. Multiple catch blocks ka ORDER kaisa ho (specific pehle, general baad me)
//    2. `throw;` (rethrow) — error ko catch karke, log karke, WAPAS aage bhejna
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ RULE: SPECIFIC pehle, GENERAL (base) sabse aakhri me                  │
//  │                                                                          │
//  │  Catch blocks UPAR se NEECHE check hote hain, aur PEHLA matching jeet     │
//  │  jaata hai. `std::exception` (base) sabko match karta hai — agar use      │
//  │  SABSE UPAR likh diya, to neeche wale specific catch (out_of_range,       │
//  │  runtime_error) tak control pahunchega hi NAHI (dead/unreachable code).   │
//  │                                                                          │
//  │     ✅ Sahi order:            ❌ Galat order:                             │
//  │        catch(out_of_range)       catch(exception)   <- sab yahin fas gaye │
//  │        catch(runtime_error)      catch(out_of_range) <- kabhi nahi chalega│
//  │        catch(exception)          catch(runtime_error)<- kabhi nahi chalega│
//  │                                                                          │
//  │  (g++ isse "-Woverloaded-virtual"/unreachable warning bhi de sakta hai.) │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `throw;` (bina argument) = REthrow — "log karke aage badha do"        │
//  │                                                                          │
//  │  Kabhi hum error ko yahan PURA handle nahi karna chahte — bas log karna   │
//  │  chahte hain (telemetry, rollback), aur asli handling upar wali layer pe  │
//  │  chhodni hai. `throw;` current exception ko jaisa-ka-tesa (type + data    │
//  │  intact) dobara phenk deta hai. ⚠ `throw ex;` mat likhna — wo COPY banata │
//  │  hai aur derived type "slice" (kat) sakta hai. Sirf `throw;` (khaali).    │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#include <iostream>
#include <stdexcept>

using namespace std;

// throwRuntime true -> runtime_error, false -> out_of_range.
void riskyOperation(bool throwRuntime) {
    if (throwRuntime) {
        throw runtime_error("something failed at runtime");
    }
    throw out_of_range("index invalid");
}

int main() {
    cout << "=== 04 Catch order & rethrow ===\n\n";

    // ---- Part 1: sahi catch order (specific -> general) --------------------
    try {
        riskyOperation(false); // out_of_range throw karega
    }
    catch (const out_of_range &ex) { // ✅ specific pehle
        cout << "Specific catch (out_of_range): " << ex.what() << "\n";
    }
    catch (const runtime_error &ex) { // specific
        cout << "Specific catch (runtime_error): " << ex.what() << "\n";
    }
    catch (const exception &ex) { // ✅ general (base) sabse aakhri me — safety net
        cout << "General catch (exception): " << ex.what() << "\n";
    }

    // ---- Part 2: rethrow (log + propagate) ---------------------------------
    cout << "\n--- Rethrow: log + propagate ---\n";
    try {
        try {
            riskyOperation(true); // runtime_error throw
        }
        catch (const runtime_error &ex) {
            // Yahan sirf LOG kiya (rollback/telemetry), handle NAHI kiya poora.
            cout << "Inner log: " << ex.what() << " (Yahan error log kar diya)\n";
            throw; // ⭐ REthrow — wahi exception aage (outer block) ko de do
        }
    }
    catch (const runtime_error &ex) {
        // Asli handling yahan (outer/upper layer).
        cout << "Outer handled after rethrow: " << ex.what() << " (Yahan properly handle ho gaya)\n";
    }

    return 0;
}

// Expected output:
//   Specific catch (out_of_range): index invalid
//   Inner log: something failed at runtime (Yahan error log kar diya)
//   Outer handled after rethrow: something failed at runtime (Yahan properly handle ho gaya)
