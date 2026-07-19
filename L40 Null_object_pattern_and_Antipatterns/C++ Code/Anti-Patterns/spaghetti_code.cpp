/*
=====================================================================
SPAGHETTI CODE ANTI-PATTERN
=====================================================================
THEORY:
Jab classes ek dusre pe itna zyada depend karne lagti hain ki unka
flow samajhna mushkil ho jata hai - bilkul spaghetti noodles jaisa
ulja hua - to ise Spaghetti Code kehte hain.

Yahan Circular Dependency ka example hai: A -> B -> C -> A
(A, B ko call karti hai; B, C ko call karti hai; aur C wapas A ko
call kar deti hai - ek loop jaisa structure ban jata hai)

PROBLEM:
- Code ka flow trace karna mushkil hota hai
- Compile-time issues aa sakte hain (forward declarations ki zaroorat)
- Har class dusri class se tightly coupled ho jati hai
- Ek class change karo to baaki sab pe asar padta hai

SOLUTION APPROACH:
- Dependency Inversion Principle use karo (interfaces/abstract classes)
- Event-driven architecture ya Mediator Pattern use karo taaki
  classes seedhe ek dusre ko na bulayein, balki ek "mediator" ke
  through communicate karein
=====================================================================
*/

#include <iostream>
using namespace std;

// ---------------------------------------------------------------
// ANTI-PATTERN VERSION: circular dependency A -> B -> C -> A
// ---------------------------------------------------------------
class A;
class B;
class C; // Forward declarations - C++ me zaroori hai jab classes
         // ek dusre ko reference karti hain

class A
{
public:
    B *b;
    void setB(B *bObj) { b = bObj; }
    void doA();
};

class B
{
public:
    C *c;
    void setC(C *cObj) { c = cObj; }
    void doB()
    {
        cout << "B is doing its work" << endl;
    }
};

class C
{
public:
    A *a; // Circular dependency yahan aakar loop bana rahi hai
    void setA(A *aObj) { a = aObj; }
    void doC()
    {
        cout << "C is doing its work, calling back to A" << endl;
        // a->doA(); // isko call karna hi ek loop create kar dega
    }
};

void A::doA()
{
    cout << "A is doing its work, calling B" << endl;
    b->doB();
}

// ---------------------------------------------------------------
// FIXED VERSION: Mediator Pattern se circular dependency hataya
// ---------------------------------------------------------------
class Mediator
{
public:
    void notify(string sender, string event)
    {
        cout << "[Mediator] " << sender << " triggered event: " << event << endl;
        // Yahan mediator decide karega ki kis component ko kya karna hai,
        // components ek dusre ko directly nahi bulate
        if (sender == "A" && event == "start")
        {
            cout << "[Mediator] Telling B to do its work" << endl;
        }
        else if (sender == "B" && event == "finished")
        {
            cout << "[Mediator] Telling C to do its work" << endl;
        }
    }
};

class ComponentA
{
    Mediator *mediator;

public:
    ComponentA(Mediator *m) : mediator(m) {}
    void trigger()
    {
        cout << "ComponentA started" << endl;
        mediator->notify("A", "start");
    }
};

int main()
{
    cout << "--- Spaghetti Code (Anti-Pattern) Demo ---" << endl;
    A objA;
    B objB;
    C objC;
    objA.setB(&objB);
    objB.setC(&objC);
    objC.setA(&objA);
    objA.doA();

    cout << "\n--- Fixed Version (Mediator Pattern) Demo ---" << endl;
    Mediator mediator;
    ComponentA compA(&mediator);
    compA.trigger();

    return 0;
}