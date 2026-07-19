// C++17 — catch(...) blocks, throwing non-exception types (avoid), and uncaught exception -> terminate behavior
#include <bits/stdc++.h>
using namespace std;

// BAD PRACTICE: C++ me kisi bhi datatypes (jaise int, double, raw pointers, strings) ko throw kiya ja sakta hai.
// Par design guidelines ke mutabik, hamesha `std::exception` se inherited class objects hi throw karna chahiye.
void throwInt() { 
    throw 42; // C++ allows this, but it is a very bad practice.
}

void throwUnknown() { 
    throw "literal C-string"; // Raw character pointers strings trigger. Avoid this.
}

int main() {
    cout << "=== 12 catch(...) & bad throws ===\n\n";

    cout << "--- catch(...) swallows everything ---\n";
    try {
        throwInt();
    } 
    // invalid matches
    catch (const exception &) {
        cout << "Never prints for throw 42 (since int does not inherit std::exception)\n";
    } 
    // catch-all block: `catch(...)` C++ me har tarah ke object/primitive values ko match and catch kar sakta hai.
    catch (...) {
        cout << "catch(...) got unknown type (int 42 successfully caught)\n";
    }

    try {
        throwUnknown();
    } 
    catch (...) {
        cout << "catch(...) got C-string throw successfully\n";
    }

    cout << "\n--- Always catch const exception& when possible ---\n";
    try {
        throw runtime_error("good practice: inherits std::exception hierarchy");
    } 
    // Standard catch pattern.
    catch (const exception &ex) {
        cout << "Proper catch: " << ex.what() << "\n";
    }

    cout << "\n--- Uncaught exception -> std::terminate (Abort) ---\n";
    cout << "Agar system me koi exception throw hui aur uske calls path me koi matching catch block nahi mila,\n";
    cout << "toh runtime environment default standard `std::terminate()` call karta hai aur process abort ho jata hai.\n";

    return 0;
}
