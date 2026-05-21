// C++17 — g++ -std=c++17 -Wall -Wextra "C++ Code/01_basics_try_catch.cpp" -o bin/01_basics_try_catch
#include <bits/stdc++.h>
using namespace std;

// Demo 1: throw, try, catch — control flow jump karta hai

double divide(double a, double b) {
    if (b == 0) {
        throw invalid_argument("divide(): divisor cannot be zero");
    }
    return a / b;
}

int main() {
    cout << "=== 01 Basics: try / catch / throw ===\n";

    try {
        cout << "10 / 2 = " << divide(10, 2) << "\n";
        cout << "10 / 0 = " << divide(10, 0) << "\n"; // never prints
        cout << "This line will NOT run\n";
    } catch (const invalid_argument &ex) {
        cout << "Caught: " << ex.what() << "\n";
    }

    cout << "Program continues after catch block\n";
    return 0;
}
