#include <iostream>
#include <typeinfo>
#include <string>
using namespace std;

class Animal {
public:
    virtual void speak() const { cout << "Animal\n"; }
    virtual ~Animal() = default;
};

class Dog : public Animal {
public:
    void speak() const override { cout << "Dog\n"; }
    void fetch() const { cout << "fetch!\n"; }
};

class Cat : public Animal {
public:
    void speak() const override { cout << "Cat\n"; }
};

// Non-polymorphic — no vtable, limited RTTI
class Plain {
public:
    int x = 1;
};

int main() {
    Dog d;
    Cat c;
    Animal* pDog = &d;
    (void)&c;

    cout << "=== typeid — runtime type name ===\n";
    cout << "  typeid(d).name()        = " << typeid(d).name() << endl;
    cout << "  typeid(*pDog).name()    = " << typeid(*pDog).name() << "  (pointee — dynamic)\n";
    cout << "  typeid(pDog).name()     = " << typeid(pDog).name() << "  (pointer static type Animal*)\n";
    cout << "  typeid(*pDog)==typeid(d)= " << (typeid(*pDog) == typeid(d) ? "yes" : "no") << endl;

    cout << "\n=== typeid + dynamic_cast pattern (safe downcast) ===\n";
    Animal* animals[] = { &d, &c };
    for (Animal* a : animals) {
        cout << "  object type: " << typeid(*a).name();
        if (Dog* dg = dynamic_cast<Dog*>(a)) {
            cout << " → Dog, calling fetch: ";
            dg->fetch();
        } else {
            cout << " → not Dog\n";
        }
    }

    cout << "\n=== dynamic_cast requires polymorphic base (≥1 virtual) ===\n";
    // dynamic_cast<Plain*> — only works with polymorphic types in hierarchy

    cout << "\n=== Non-polymorphic class ===\n";
    Plain plain;
    cout << "  typeid(plain).name() = " << typeid(plain).name() << " (compile-time/static)\n";

    cout << "\n=== RTTI enable / disable (build flags) ===\n";
    cout << "  Default (g++/clang): RTTI ON — dynamic_cast & typeid work on polymorphic types.\n";
    cout << "  g++ -fno-rtti       : dynamic_cast / typeid on polymorphic types → compile error or limited.\n";
    cout << "  Use -fno-rtti in embedded/size-critical builds; prefer static typing / variant instead.\n";

#if __has_feature(cxx_rtti) || defined(__GXX_RTTI) || defined(_CPPRTTI)
    cout << "  This build: RTTI enabled (__cplusplus RTTI macros).\n";
#else
    cout << "  This build: RTTI may be disabled.\n";
#endif

    return 0;
}
