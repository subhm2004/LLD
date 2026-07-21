// ============================================================================
//  09_cpp17_features.cpp  —  Modern C++17: exceptions ke ALTERNATIVES + syntax
// ----------------------------------------------------------------------------
//  Build: g++ -std=c++17 -Wall -Wextra "C++ Code/09_cpp17_features.cpp" -o bin/09_cpp17_features
//
//  Har error ke liye exception phenkna aksar OVERKILL hai (exceptions "mehnge"
//  hote hain — throw/unwind slow hai). C++17 ke kuch features hume "expected"
//  failures ko exception ke BINA handle karne dete hain.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SABSE BADI SEEKH — kab optional, kab exception?                       │
//  │                                                                          │
//  │   std::optional  -> jab failure NORMAL/EXPECTED ho (key missing, balance  │
//  │                     kam, cache miss). "Mila ya nahi" wala natija. Sasta.  │
//  │   Exception      -> jab state EXTREME/INVALID ho (corrupt data, system    │
//  │                     fault, "aisa hona hi nahi chahiye tha"). Mehenga.     │
//  │                                                                          │
//  │  Rule of thumb: "exception should be EXCEPTIONAL" — normal control flow   │
//  │  ke liye exceptions mat use karo.                                         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  Is file me 4 C++17 features dikhaye gaye hain:
//     1. std::optional          — "value ya kuch nahi" (nullopt)
//     2. std::string_view       — string ka lightweight non-owning view
//     3. if-with-initializer    — if(init; condition) — scope tight rakhta hai
//     4. structured bindings     — auto& [key, value] — pair/struct destructure
//
//  📌 Notice: ye file pehle se targeted includes use karti thi (bits/stdc++.h
//     nahi) — accha modern style. Baaki files ko bhi isi style pe laaya gaya.
// ============================================================================
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

using namespace std;

// ---- Feature 1: std::optional — failure ko exception ke bina represent karo --
class Wallet {
public:
    // ⭐ tryWithdraw exception NAHI phenkta. Success -> optional me balance,
    //    failure (invalid/insufficient) -> nullopt (khaali). Caller check karta.
    optional<double> tryWithdraw(double amount) {
        if (amount <= 0) {
            return nullopt; // failure (expected) — koi throw nahi
        }
        if (amount > balance_) {
            return nullopt; // insufficient (expected) — koi throw nahi
        }
        balance_ -= amount;
        return balance_; // success -> value wrap karke do
    }

    double getBalance() const { return balance_; }

private:
    double balance_ = 1000.0;
};

// ---- Feature 2: std::string_view — string ki COPY banaye bina "dekho" --------
// Read-only parameter ke liye string_view lo -> string copy nahi banti (fast).
optional<string_view> lookupFileName(const unordered_map<string, string> &files, string_view path) {
    auto it = files.find(string(path)); // (map ki key string hai, isliye yahan convert)
    if (it == files.end()) {
        return nullopt;
    }
    return string_view(it->second);
}

int main() {
    cout << "=== 09 C++17 features (optional / string_view / structured bindings) ===\n\n";

    Wallet wallet;

    // ---- Feature 3: if-with-initializer -> if (init; condition) -------------
    // `left` sirf is if-else ke andar zinda rehta (bahar leak nahi karta — clean scope).
    if (optional<double> left = wallet.tryWithdraw(200); left.has_value()) {
        cout << "Withdraw OK, balance left: " << *left << "\n"; // *left = value nikaalo
    } else {
        cout << "Withdraw failed (optional empty — no exception)\n";
    }

    if (optional<double> left = wallet.tryWithdraw(9999); left.has_value()) {
        cout << "Should not print\n";
    } else {
        cout << "Large withdraw rejected via optional\n"; // 9999 > balance -> nullopt
    }

    unordered_map<string, string> files{{"/a.txt", "content-A"}, {"/b.txt", "content-B"}};

    if (auto name = lookupFileName(files, "/a.txt"); name.has_value()) {
        cout << "lookup /a.txt -> " << *name << "\n";
    }

    // ---- Feature 4: structured bindings -> auto& [key, value] ---------------
    // Map ke har pair ko seedha do naam me tod do (`.first`/`.second` likhne ki jagah).
    for (const auto &[path, content] : files) {
        cout << "file " << path << " size=" << content.size() << "\n";
    }

    cout << "\nTip: `std::optional` tab use karo jab failure common/expected ho; exceptions tab jab state extreme-invalid ho.\n";
    return 0;
}
