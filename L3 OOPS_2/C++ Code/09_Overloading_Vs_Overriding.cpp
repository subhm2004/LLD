/**
 * Overloading (static) vs Overriding (dynamic) — side by side
 */
#include <iostream>
using namespace std;

class Printer {
public:
    // STATIC — overload: same class, different signature
    void print(const string& msg) {
        cout << "[overload] string: " << msg << "\n";
    }
    void print(int code) {
        cout << "[overload] int: " << code << "\n";
    }
};

class Base {
public:
    virtual void execute() { cout << "[override] Base::execute\n"; }
    virtual ~Base() = default;
};

class Derived : public Base {
public:
    void execute() override { cout << "[override] Derived::execute\n"; }
};

int main() {
    cout << "=== OVERLOADING (compile-time, same class) ===\n";
    Printer p;
    p.print("hello");
    p.print(404);

    cout << "\n=== OVERRIDING (runtime, inheritance + virtual) ===\n";
    Base* b = new Derived();
    b->execute();
    delete b;

    cout << "\n| Overloading | Overriding |\n"
         << "| compile time | runtime |\n"
         << "| same class | parent/child |\n"
         << "| diff signature | same signature |\n";
    return 0;
}
