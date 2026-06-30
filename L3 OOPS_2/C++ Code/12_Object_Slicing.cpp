// ============================================================================
//  12_Object_Slicing.cpp  —  Object slicing problem
// ----------------------------------------------------------------------------
//  Object slicing = jab derived object ko base object me BY VALUE copy/assign
//  karte ho, to derived-specific part "kat" jaata hai (slice ho jaata hai),
//  sirf base part bachta hai -> polymorphism toot jaata hai. Bachne ka tareeka:
//  hamesha base POINTER ya REFERENCE use karo (Base&/Base*), by-value nahi.
// ============================================================================
#include <iostream>
#include <string>
using namespace std;

class Animal {
public:
    string name = "Animal";
    virtual void speak() const { cout << name << " [Animal::speak] generic sound\n"; }
    virtual ~Animal() = default;
};

class Dog : public Animal {
    string breed = "Labrador";  // extra data — lost on slice
public:
    Dog() { name = "Dog"; }
    void speak() const override {
        cout << name << " [Dog::speak] barks (breed=" << breed << ")\n";
    }
    void fetch() const { cout << breed << " fetches the ball\n"; }
};

int main() {
    Dog d;
    cout << "sizeof(Animal)=" << sizeof(Animal) << " sizeof(Dog)=" << sizeof(Dog) << endl;

    cout << "\n--- Object slicing (by value) ---\n";
    Animal sliced = d;           // ❌ Dog part + vptr type sliced off
    cout << "After Animal sliced = d:\n";
    sliced.speak();              // Animal::speak — NOT Dog::speak

    cout << "\n--- Correct: pointer / reference ---\n";
    Animal* p = &d;
    Animal& r = d;
    p->speak();                  // Dog::speak (polymorphism)
    r.speak();

    cout << "\n--- Slicing via function by value ---\n";
    auto byValue = [](Animal a) {  // pass-by-value copies Animal part only
        cout << "Inside byValue param: ";
        a.speak();
    };
    byValue(d);

    cout << "\n--- Fix: pass by reference ---\n";
    auto byRef = [](const Animal& a) {
        cout << "Inside byRef param: ";
        a.speak();
    };
    byRef(d);

    cout << "\nAvoid: Base b = derivedObj;  Prefer: Base& or Base* or unique_ptr<Base>.\n";
    return 0;
}
