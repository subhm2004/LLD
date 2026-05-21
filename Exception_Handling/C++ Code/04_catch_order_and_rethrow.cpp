// C++17 — g++ -std=c++17 "C++ Code/04_catch_order_and_rethrow.cpp" -o bin/04_catch_order_and_rethrow
#include <bits/stdc++.h>
using namespace std;

// Demo 4: Catch order (specific -> general) + rethrow
// Galat order: catch(const exception&) PEHLE likho to out_of_range handler kabhi nahi chalega

void riskyOperation(bool throwRuntime) {
    if (throwRuntime) {
        throw runtime_error("something failed at runtime");
    }
    throw out_of_range("index invalid");
}

int main() {
    cout << "=== 04 Catch order & rethrow ===\n\n";

    // Wrong order example (commented): catch(exception) pehle -> specific kabhi nahi chalega
    try {
        riskyOperation(false);
    } catch (const out_of_range &ex) {
        cout << "Specific catch (out_of_range): " << ex.what() << "\n";
    } catch (const runtime_error &ex) {
        cout << "Specific catch (runtime_error): " << ex.what() << "\n";
    } catch (const exception &ex) {
        cout << "General catch (exception): " << ex.what() << "\n";
    }

    cout << "\n--- Rethrow: log + propagate ---\n";
    try {
        try {
            riskyOperation(true);
        } catch (const runtime_error &ex) {
            cout << "Inner log: " << ex.what() << "\n";
            throw; // rethrow same exception
        }
    } catch (const runtime_error &ex) {
        cout << "Outer handled after rethrow: " << ex.what() << "\n";
    }

    return 0;
}
