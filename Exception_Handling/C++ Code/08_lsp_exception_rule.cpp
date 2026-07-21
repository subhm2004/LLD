// ============================================================================
//  08_lsp_exception_rule.cpp  —  LSP ka exception rule (override kya throw kare)
// ----------------------------------------------------------------------------
//  Build: g++ -std=c++17 -Wall -Wextra "C++ Code/08_lsp_exception_rule.cpp" -o bin/08_lsp_exception_rule
//
//  LSP = Liskov Substitution Principle: "child object ko parent ki jagah rakho
//  to code TOOTNA nahi chahiye." Exceptions ke liye iska matlab:
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ OVERRIDE method parent se WIDER (broader) exception THROW nahi kar    │
//  │     sakta — same ya NARROWER (subclass) hi chalega                       │
//  │                                                                          │
//  │  Kyun? Client parent ka contract dekh ke catch likhta hai. Agar parent   │
//  │  bola "main sirf logic_error phenkunga", to client `catch(logic_error)`  │
//  │  likhega. Ab agar child chupke se `runtime_error` (jo logic_error ka      │
//  │  parallel/wider hai) phenk de, to client ka catch use MISS kar dega ->    │
//  │  unhandled exception -> contract TOOT gaya.                              │
//  │                                                                          │
//  │     Parent throws:  logic_error                                          │
//  │     GoodChild:      invalid_argument  (logic_error ka CHILD = narrower) ✅ │
//  │     BadChild:       runtime_error     (logic_error se WIDER/parallel)   ❌ │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Yaad rakhne ka tareeka: child method exceptions ko "sikod" (narrow) sakta
//     hai, "phaila" (widen) nahi. Bilkul noexcept wale rule jaisa (file 06):
//     guarantee ko TIGHT karna OK, LOOSE karna galat.
//
//  ⚠ C++ ye rule COMPILER se enforce nahi karta (Java ke `throws` clause ki
//     tarah nahi) — ye ek DESIGN discipline hai. Isi demo me BadChild compile
//     to ho jaata hai, par client ka logic_error-wala catch use miss kar deta.
// ============================================================================
#include <iostream>
#include <stdexcept>

using namespace std;

// Parent ka "contract": getValue sirf logic_error family phenkega.
class Parent {
public:
    virtual void getValue() { throw logic_error("Parent logic_error"); }
    virtual ~Parent() = default;
};

// ✅ GoodChild: invalid_argument phenkta hai — jo logic_error ka SUBCLASS hai
//    (narrower). Client ka catch(logic_error) ise AAP HI pakad lega. LSP safe.
class GoodChild : public Parent {
public:
    void getValue() override {
        throw invalid_argument("narrower: invalid_argument is a subclass of logic_error");
    }
};

// ❌ BadChild: runtime_error phenkta hai — jo logic_error se WIDER/parallel hai
//    (uska subclass nahi). Client ka catch(logic_error) ise MISS kar dega. LSP violated.
class BadChild : public Parent {
public:
    void getValue() override {
        throw runtime_error("BROADER than logic_error — breaks client expecting logic_error");
    }
};

// Client jo Parent (polymorphically) use karta hai aur logic_error expect karta hai.
class Client {
public:
    explicit Client(Parent *p) : p_(p) {}

    void takeValue() {
        try {
            p_->getValue();
        }
        // Client ka asli, intended handler — logic_error ke liye.
        catch (const logic_error &ex) {
            cout << "Client caught logic_error: " << ex.what() << "\n";
        }
        // ⭐ Ye "safety net" sirf isliye hai taaki demo crash na kare. Asli code me
        //    BadChild ka runtime_error yahan girega = LSP toot gaya (intended handler miss).
        catch (const exception &ex) {
            cout << "Client caught OTHER exception (LSP Violated): " << ex.what() << "\n";
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
    Client(&parent).takeValue(); // logic_error -> intended catch

    cout << "\nGoodChild execution (Narrower exception -> OK):\n";
    Client(&good).takeValue(); // invalid_argument (child of logic_error) -> intended catch

    cout << "\nBadChild execution (runtime_error -> LSP Violated):\n";
    Client(&bad).takeValue(); // runtime_error -> girta hai OTHER catch me (violation)

    return 0;
}

// Expected output:
//   Parent:    Client caught logic_error: Parent logic_error
//   GoodChild: Client caught logic_error: narrower: invalid_argument ...
//   BadChild:  Client caught OTHER exception (LSP Violated): BROADER than logic_error ...
