// C++17 — g++ -std=c++17 -Wall -Wextra "C++ Code/08_lsp_exception_rule.cpp" -o bin/08_lsp_exception_rule
#include <bits/stdc++.h>
using namespace std;

// Demo 8: LSP — child should not throw broader exceptions than parent expects
// (Same idea as L6 SOLID_2 ExceptionRule.cpp)

class Parent {
public:
    virtual void getValue() { throw logic_error("Parent logic_error"); }
};

class GoodChild : public Parent {
public:
    void getValue() override {
        throw invalid_argument("narrower: invalid_argument is a logic_error");
    }
};

class BadChild : public Parent {
public:
    void getValue() override {
        throw runtime_error("BROADER than logic_error — breaks client expecting logic_error");
    }
};

class Client {
public:
    explicit Client(Parent *p) : p_(p) {}

    void takeValue() {
        try {
            p_->getValue();
        } catch (const logic_error &ex) {
            cout << "Client caught logic_error: " << ex.what() << "\n";
        } catch (const exception &ex) {
            cout << "Client caught other exception: " << ex.what() << "\n";
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

    cout << "Parent:\n";
    Client(&parent).takeValue();

    cout << "\nGoodChild (narrower exception — OK):\n";
    Client(&good).takeValue();

    cout << "\nBadChild (runtime_error — client logic_error catch misses):\n";
    Client(&bad).takeValue();

    return 0;
}
