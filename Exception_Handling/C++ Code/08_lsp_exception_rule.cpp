// C++17 — g++ -std=c++17 -Wall -Wextra "C++ Code/08_lsp_exception_rule.cpp" -o bin/08_lsp_exception_rule
#include <bits/stdc++.h>
using namespace std;

// Demo 8: Liskov Substitution Principle (LSP) — Child class method ko parent contract se wider exceptions throw nahi karna chahiye.

// Parent class: contract set karti hai ki `getValue` sirf `logic_error` throw karegi.
class Parent {
public:
    virtual void getValue() { 
        throw logic_error("Parent logic_error"); 
    }
};

// Good Child: LSP follow kar raha hai.
// Kyunki `invalid_argument` child class hai `logic_error` ki (Narrower exception).
// Client jo `logic_error` expect kar raha hai, wo is narrow range ko handle kar lega.
class GoodChild : public Parent {
public:
    void getValue() override {
        throw invalid_argument("narrower: invalid_argument is a subclass of logic_error");
    }
};

// Bad Child: LSP violate kar raha hai.
// Kyunki `runtime_error` standard hierarchy me `logic_error` se direct parallel/wider class hai, ye logic_error branch me nahi aati.
// Agar client sirf `logic_error` target kar raha hai, toh runtime_error handle nahi ho payega aur client contract break ho jayega.
class BadChild : public Parent {
public:
    void getValue() override {
        throw runtime_error("BROADER than logic_error — breaks client expecting logic_error");
    }
};

// Client class jo Parent object consume karti hai polymorphism ke through.
class Client {
public:
    explicit Client(Parent *p) : p_(p) {}

    void takeValue() {
        try {
            p_->getValue();
        } 
        // Client expects and safely handles logic_error
        catch (const logic_error &ex) {
            cout << "Client caught logic_error: " << ex.what() << "\n";
        } 
        // Dusre exceptions catch all me chale jayenge
        catch (const exception &ex) {
            cout << "Client caught other exception (LSP Violated): " << ex.what() << "\n";
        }
    }

private:
    Parent *p_;
};

int main() {
    cout << "=== 08 LSP exception rule ===\n\n";

    Parent parent;
    GoodChild good;
    BadChild bad;

    cout << "Parent class execution:\n";
    Client(&parent).takeValue();

    cout << "\nGoodChild execution (Narrower exception -> OK):\n";
    Client(&good).takeValue();

    cout << "\nBadChild execution (runtime_error -> LSP Violated):\n";
    Client(&bad).takeValue();

    return 0;
}
