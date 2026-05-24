/**
 * Virtual functions & vtable (conceptual)
 * Compiler adds vptr; virtual calls go through vtable at runtime
 */
#include <iostream>
using namespace std;

class Animal {
public:
    virtual void speak() { cout << "Animal\n"; }
    virtual ~Animal() = default;
};

class Dog : public Animal {
public:
    void speak() override { cout << "Woof\n"; }
};

class Cat : public Animal {
public:
    void speak() override { cout << "Meow\n"; }
};

void makeSpeak(Animal* a) {
    a->speak();  // runtime dispatch via vtable
}

int main() {
    Animal* d = new Dog();
    Animal* c = new Cat();

    cout << "sizeof(Animal): " << sizeof(Animal)
         << " (includes hidden vptr on most compilers)\n";

    makeSpeak(d);
    makeSpeak(c);

    delete d;
    delete c;

    cout << "\nvptr → vtable → correct speak() at runtime\n";
    return 0;
}
