// C++17 demo — optional, string_view, structured bindings, if-init
// Build: g++ -std=c++17 -Wall -Wextra "C++ Code/09_cpp17_features.cpp" -o bin/09_cpp17_features

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

using namespace std;

class Wallet {
public:
    optional<double> tryWithdraw(double amount) {
        if (amount <= 0) {
            return nullopt;
        }
        if (amount > balance_) {
            return nullopt;
        }
        balance_ -= amount;
        return balance_;
    }

    double getBalance() const { return balance_; }

private:
    double balance_ = 1000.0;
};

// C++17: string_view — no copy for read-only path
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

    // if-init (C++17)
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

    // structured bindings (C++17)
    for (const auto &[path, content] : files) {
        cout << "file " << path << " size=" << content.size() << "\n";
    }

    cout << "\nLLD tip: optional = expected failure; exception = exceptional / invalid state\n";
    return 0;
}
