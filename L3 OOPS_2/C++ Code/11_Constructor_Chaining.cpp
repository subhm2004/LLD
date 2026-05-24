#include <iostream>
#include <string>
using namespace std;

class Base {
    string name;
public:
    Base() : name("default-Base") {
        cout << "  1) Base() default — name=" << name << endl;
    }
    explicit Base(const string& n) : name(n) {
        cout << "  1) Base(string) — name=" << name << endl;
    }
    virtual ~Base() {
        cout << "  4) ~Base() — name=" << name << endl;
    }
};

class Derived : public Base {
    int id;
public:
    // Child MUST initialize Base part first (initializer list)
    Derived(int id, const string& baseName)
        : Base(baseName),   // explicit parent ctor — NOT automatic with args
          id(id) {
        cout << "  2) Derived body — id=" << id << endl;
    }

    Derived() : Base(), id(0) {  // chains to Base()
        cout << "  2) Derived() default — id=0" << endl;
    }

    ~Derived() override {
        cout << "  3) ~Derived() — id=" << id << endl;
    }
};

class GrandChild : public Derived {
public:
    GrandChild() : Derived(99, "via-GrandChild") {
        cout << "  2b) GrandChild body" << endl;
    }
    ~GrandChild() override {
        cout << "  3b) ~GrandChild()" << endl;
    }
};

int main() {
    cout << "=== Order: Base subobject → Derived → (GrandChild) ===\n";
    {
        cout << "Creating GrandChild:\n";
        GrandChild g;
        cout << "Destroying (reverse order):\n";
    }

    cout << "\n=== Without Base(...) in list — only works if Base has default ctor ===\n";
    {
        cout << "Creating Derived():\n";
        Derived d;
        cout << "Destroying:\n";
    }

    cout << "\nRule: Parent subobject constructed BEFORE child body runs.\n";
    cout << "Destructors: child first, then parent (reverse).\n";
    return 0;
}
