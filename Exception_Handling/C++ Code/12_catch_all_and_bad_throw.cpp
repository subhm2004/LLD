// C++17 — catch(...), throwing non-exception types (avoid), uncaught -> terminate
#include <bits/stdc++.h>
using namespace std;

void throwInt() { throw 42; } // BAD practice — don't throw int

void throwUnknown() { throw "literal C-string"; } // also bad

int main() {
    cout << "=== 12 catch(...) & bad throws ===\n\n";

    cout << "--- catch(...) swallows everything ---\n";
    try {
        throwInt();
    } catch (const exception &) {
        cout << "Never prints for throw 42\n";
    } catch (...) {
        cout << "catch(...) got unknown type (int 42)\n";
    }

    try {
        throwUnknown();
    } catch (...) {
        cout << "catch(...) got C-string throw\n";
    }

    cout << "\n--- Always catch const exception& when possible ---\n";
    try {
        throw runtime_error("good: inherits std::exception");
    } catch (const exception &ex) {
        cout << "Proper catch: " << ex.what() << "\n";
    }

    cout << "\n--- Uncaught exception -> std::terminate (not run here) ---\n";
    cout << "If you uncomment next line, program aborts:\n";
    cout << "// throw runtime_error(\"uncaught\");\n";

    return 0;
}
