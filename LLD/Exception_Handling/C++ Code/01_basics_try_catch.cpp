// ============================================================================
//  01_basics_try_catch.cpp  —  Exception handling ka ABC: throw / try / catch
// ----------------------------------------------------------------------------
//  Build: g++ -std=c++17 -Wall -Wextra "C++ Code/01_basics_try_catch.cpp" -o bin/01_basics_try_catch
//
//  Exception handling = "kuch galat ho to normal flow rok ke, error ko ek
//  alag jagah handle karo" — bina program crash kiye.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ TEEN KEYWORD, TEEN KAAM                                                │
//  │                                                                          │
//  │     throw  -> "problem hai!" — error object phenk do                      │
//  │     try    -> "ye code risky hai" — ispe nazar rakho                      │
//  │     catch  -> "problem aaye to ye karo" — error ko sambhaalo              │
//  │                                                                          │
//  │  Jaise hi `throw` chalta hai, normal execution RUK jaati hai aur control  │
//  │  seedha uparne wale matching `catch` block pe JUMP kar jaata hai. Beech   │
//  │  ka koi code (jaise throw ke baad wali lines) chalta hi nahi.            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ KYUN? (return-code se behtar kyun): purane style me function -1 ya false
//     lauta ta tha aur caller ko HAR baar check karna padta ("agar -1 to...").
//     Exception apne aap "propagate" hoti hai — jahan handle karna ho wahin
//     catch karo, beech ke functions ko error-check ka bojh nahi.
// ============================================================================
#include <iostream>
#include <stdexcept>

using namespace std;

// Do numbers divide karta hai. Divisor 0 ho to division possible nahi ->
// `invalid_argument` (ek standard exception) throw karke saaf mana kar dete hain.
double divide(double a, double b) {
    if (b == 0) {
        // ⭐ throw = yahin se normal flow khatam. Control matching catch dhoondhne
        //    nikal jaata hai (is function ke neeche `return a/b` NAHI chalega).
        throw invalid_argument("divide(): divisor cannot be zero");
    }
    return a / b;
}

int main() {
    cout << "=== 01 Basics: try / catch / throw ===\n";

    try {
        // try block ke andar wahi code jo throw kar sakta hai.
        cout << "10 / 2 = " << divide(10, 2) << "\n"; // ye theek chalega (5)

        // ⭐ Ye call andar throw karega -> agli do lines KABHI print nahi hongi.
        cout << "10 / 0 = " << divide(10, 0) << "\n"; // <- yahan throw hua
        cout << "This line will NOT run\n";           // control yahan aata hi nahi
    }
    // ⭐ catch exception TYPE se match karta hai. `const &` se pakadna best practice
    //    hai (copy nahi banti, aur polymorphism sahi kaam karta hai).
    catch (const invalid_argument &ex) {
        // ex.what() = throw ke waqt diya gaya message.
        cout << "Caught: " << ex.what() << "\n";
    }

    // ⭐ Exception CATCH ho gayi -> program crash NAHI hua, aage smoothly chalta hai.
    //    Yahi exception handling ka poora point hai: fail ho to bhi control me raho.
    cout << "Program continues after catch block\n";
    return 0;
}

// Expected output:
//   === 01 Basics: try / catch / throw ===
//   10 / 2 = 5
//   Caught: divide(): divisor cannot be zero
//   Program continues after catch block
