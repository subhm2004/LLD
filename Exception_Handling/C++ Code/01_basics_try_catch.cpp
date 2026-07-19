// C++17 — g++ -std=c++17 -Wall -Wextra "C++ Code/01_basics_try_catch.cpp" -o bin/01_basics_try_catch
#include <bits/stdc++.h>
using namespace std;

// Demo 1: throw, try, catch — control flow jump kaise karta hai

/**
 * @brief Do numbers ko divide karta hai.
 * 
 * Agar divisor (b) zero hai, toh arithmetic rule ke mutabik division possible nahi hai.
 * Aise case me hum validation fail karke standard exception `invalid_argument` throw karte hain.
 * 
 * @param a Dividend
 * @param b Divisor (Zero nahi hona chahiye)
 * @return double Division result
 */
double divide(double a, double b) {
    if (b == 0) {
        // Exception throw karne par control flow normal execution chhod kar 
        // sidhe matching catch block ki khoj me jump kar jata hai.
        throw invalid_argument("divide(): divisor cannot be zero");
    }
    return a / b;
}

int main() {
    cout << "=== 01 Basics: try / catch / throw ===\n";

    try {
        // Try block ke andar wo code rakhte hain jo exception throw kar sakta hai.
        cout << "10 / 2 = " << divide(10, 2) << "\n";
        
        // Ye line execute hote hi `divide` function internally exception throw karega.
        cout << "10 / 0 = " << divide(10, 0) << "\n"; // Ye line kabhi print nahi hogi!
        
        cout << "This line will NOT run\n"; // Control flow yahan tak nahi pahunchega
    } 
    // Catch block exception type ko match karta hai. const reference use karna aachhi practice hai.
    catch (const invalid_argument &ex) {
        // Exception catch hone par handle hoti hai aur custom response print hota hai.
        cout << "Caught: " << ex.what() << "\n";
    }

    // Exception successfully catch hone ki wajah se program crash nahi hota aur smoothly aage chalta hai.
    cout << "Program continues after catch block\n";
    return 0;
}
