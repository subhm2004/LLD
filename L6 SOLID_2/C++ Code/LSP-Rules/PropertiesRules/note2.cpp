// ============================================================================
//  note2.cpp  —  `final` virtual function note (LSP ke context me)
// ----------------------------------------------------------------------------
//  `final` ko ek specific VIRTUAL function pe bhi laga sakte ho -> phir koi
//  derived class us function ko OVERRIDE nahi kar sakti. Use kab: jab tum
//  guarantee karna chahte ho ki ek behavior subclasses change na karein (taaki
//  LSP/invariant na toote). Neeche Dog::sound() override ERROR dega.
//  (Yeh file intentionally error dikhati hai — concept ke liye.)
// ============================================================================
#include <iostream>
using namespace std;

class Animal
{
public:
    virtual void sound() final
    { // ❌ Override nahi kar sakte
        cout << "Animal sound" << endl;
    }
};

class Dog : public Animal
{
public:
    // ❌ ERROR: Cannot override final function
    void sound() override
    {
        cout << "Dog barks" << endl;
    }
};

int main()
{
    Animal *a = new Animal();
    a->sound();
    return 0;
}