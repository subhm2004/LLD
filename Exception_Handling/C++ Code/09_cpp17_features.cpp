// C++17 demo — std::optional, std::string_view, structured bindings, aur if-with-initializer support
// Build: g++ -std=c++17 -Wall -Wextra "C++ Code/09_cpp17_features.cpp" -o bin/09_cpp17_features

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

using namespace std;

/**
 * @class Wallet
 * @brief Wallet class jo withdrawal operations perform karti hai.
 * 
 * LLD Rule: Har error ke liye Exception throw karna costly ho sakta hai.
 * Agar koi operation naturally fail ho sakta hai (jaise balance kam hona), 
 * toh use control karne ke liye `std::optional` return karna ek behtar modern C++ approach hai (No exceptions overhead).
 */
class Wallet {
public:
    // tryWithdraw return karta hai balance agar withdraw ho gaya, warna std::nullopt (empty state).
    optional<double> tryWithdraw(double amount) {
        if (amount <= 0) {
            return nullopt; // empty state represent karta hai failure.
        }
        if (amount > balance_) {
            return nullopt; // insufficient balance.
        }
        balance_ -= amount;
        return balance_; // wrapped double value.
    }

    double getBalance() const { return balance_; }

private:
    double balance_ = 1000.0;
};

// C++17 Feature: string_view. 
// Ye ek lightweight non-owning reference hai strings ke liye. Copy operations avoid karne ke liye read-only parameters me use hota hai.
optional<string_view> lookupFileName(const unordered_map<string, string> &files, string_view path) {
    auto it = files.find(string(path));
    if (it == files.end()) {
        return nullopt;
    }
    return string_view(it->second);
}

int main() {
    cout << "=== 09 C++17 features (optional / string_view / structured bindings) ===\n\n";

    Wallet wallet;

    // C++17 Feature: if-with-initializer
    // Hum condition check ke sath hi local variable scope initialize kar sakte hain `if (init; condition)`.
    // Isse `left` variable if-else block ke bahar leaks/pollute nahi karta memory ko.
    if (optional<double> left = wallet.tryWithdraw(200); left.has_value()) {
        cout << "Withdraw OK, balance left: " << *left << "\n";
    } else {
        cout << "Withdraw failed (optional empty — no exception)\n";
    }

    if (optional<double> left = wallet.tryWithdraw(9999); left.has_value()) {
        cout << "Should not print\n";
    } else {
        cout << "Large withdraw rejected via optional\n";
    }

    unordered_map<string, string> files{{"/a.txt", "content-A"}, {"/b.txt", "content-B"}};

    if (auto name = lookupFileName(files, "/a.txt"); name.has_value()) {
        cout << "lookup /a.txt -> " << *name << "\n";
    }

    // C++17 Feature: structured bindings
    // Map entries, pairs ya structs ko direct destructure karne ke liye: `auto& [key, value]`.
    for (const auto &[path, content] : files) {
        cout << "file " << path << " size=" << content.size() << "\n";
    }

    cout << "\nLLD Tip: `std::optional` tab use karein jab failure common/expected ho (e.g. key missing, balance limit fail). Exceptions tab use karein jab state extreme invalid ya system fault ho.\n";
    return 0;
}
