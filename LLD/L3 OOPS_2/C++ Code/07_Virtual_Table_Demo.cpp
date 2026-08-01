// ============================================================================
//  07_Virtual_Table_Demo.cpp  —  vtable / vptr (dynamic dispatch andar se)
// ----------------------------------------------------------------------------
//  Dynamic polymorphism kaise kaam karta hai? Har class jisme virtual function
//  hai, uska ek VTABLE (function pointers ka table) banta hai. Har object me ek
//  hidden VPTR hota hai jo us vtable ko point karta hai. virtual call ke waqt
//  program vptr -> vtable -> sahi function dhoondh ke call karta hai (runtime).
//  Isi wajah se virtual call ka thoda overhead aur object me extra pointer hota.
// ============================================================================
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
