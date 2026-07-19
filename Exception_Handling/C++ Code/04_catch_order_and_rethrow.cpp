// C++17 — g++ -std=c++17 "C++ Code/04_catch_order_and_rethrow.cpp" -o bin/04_catch_order_and_rethrow
#include <bits/stdc++.h>
using namespace std;

// Demo 4: Catch block ka sahi order (specific -> general) aur Exception Rethrowing logic

/**
 * @brief Demo function jo runtime_error ya out_of_range exception throw kar sakta hai.
 * 
 * @param throwRuntime bool flag jo decide karta hai kaunsa error throw karna hai.
 */
void riskyOperation(bool throwRuntime) {
    if (throwRuntime) {
        throw runtime_error("something failed at runtime");
    }
    throw out_of_range("index invalid");
}

int main() {
    cout << "=== 04 Catch order & rethrow ===\n\n";

    // RULE: Humesha specific exceptions ko pehle catch karein aur base class (std::exception) ko aakhri me!
    // Agar hum catch(const exception &ex) ko sabse upar likh denge, toh niche likhe specific catch block
    // (jaise out_of_range aur runtime_error) kabhi execute hi nahi honge kyunki parent catch sab ko pehle hi pakaad lega (Unreachable code error/warning).
    try {
        riskyOperation(false);
    } 
    catch (const out_of_range &ex) {
        cout << "Specific catch (out_of_range): " << ex.what() << "\n";
    } 
    catch (const runtime_error &ex) {
        cout << "Specific catch (runtime_error): " << ex.what() << "\n";
    } 
    catch (const exception &ex) {
        cout << "General catch (exception): " << ex.what() << "\n";
    }

    cout << "\n--- Rethrow: log + propagate ---\n";
    // Rethrow logic: Jab hum exception ko catch karke kuch log karna chahte hain (jaise telemetry, transactions rollback),
    // aur uske baad exception ko dobara throw kar dete hain taaki upper layer (caller stack) ise handle kar sake.
    try {
        try {
            riskyOperation(true);
        } 
        catch (const runtime_error &ex) {
            cout << "Inner log: " << ex.what() << " (Yahan error log kar diya)\n";
            throw; // `throw;` statement bina arguments ke current exception ko exactly rethrow kar deta hai.
        }
    } 
    catch (const runtime_error &ex) {
        cout << "Outer handled after rethrow: " << ex.what() << " (Yahan upper block me properly handle ho gaya)\n";
    }

    return 0;
}
