// ============================================================================
//  14_Smart_Pointers.cpp  —  Smart pointers (unique_ptr / shared_ptr / weak_ptr)
// ----------------------------------------------------------------------------
//  Smart pointers = RAII wrappers jo memory AUTOMATIC free kar dete hain (manual
//  delete nahi). Teen types:
//    - unique_ptr : EXCLUSIVE ownership (ek hi maalik), copy nahi ho sakta.
//    - shared_ptr : SHARED ownership (reference count), last owner pe delete.
//    - weak_ptr   : shared_ptr ka non-owning observer -> circular reference
//                   (cyclic) leak ko todne ke liye.
//  Modern C++ me raw owning pointers ki jagah yahi use karo.
// ============================================================================
#include <iostream>
#include <memory>
using namespace std;

struct Widget {
    int id;
    explicit Widget(int i) : id(i) { cout << "Widget " << id << " created\n"; }
    ~Widget() { cout << "Widget " << id << " destroyed\n"; }
};

int main() {
    cout << "=== unique_ptr (exclusive ownership) ===\n";
    {
        unique_ptr<Widget> u = make_unique<Widget>(1);
        // unique_ptr<Widget> u2 = u;  // ERROR: not copyable
        unique_ptr<Widget> u2 = std::move(u);  // OK: transfer ownership
        cout << "u null? " << (u == nullptr) << "\n";
    }

    cout << "\n=== shared_ptr (shared ownership) ===\n";
    {
        shared_ptr<Widget> s1 = make_shared<Widget>(2);
        {
            shared_ptr<Widget> s2 = s1;
            cout << "use_count: " << s1.use_count() << "\n";
        }
        cout << "use_count after inner scope: " << s1.use_count() << "\n";
    }

    cout << "\n=== weak_ptr (break cycles, no ownership) ===\n";
    {
        shared_ptr<Widget> sp = make_shared<Widget>(3);
        weak_ptr<Widget> wp = sp;
        cout << "weak expired? " << wp.expired() << "\n";
        if (auto locked = wp.lock())
            cout << "locked id: " << locked->id << "\n";
        sp.reset();
        cout << "after reset, weak expired? " << wp.expired() << "\n";
    }
    return 0;
}
