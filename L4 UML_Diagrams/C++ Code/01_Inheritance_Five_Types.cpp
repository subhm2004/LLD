// ============================================================================
//  01_Inheritance_Five_Types.cpp  —  Is-A relationship ke 5 types (UML L4)
// ----------------------------------------------------------------------------
//  Inheritance = "IS-A" relationship (Dog IS-A Animal). UML me solid line +
//  hollow triangle (base ki taraf). Yeh file C++ ke 5 inheritance types
//  ek-ek karke dikhati hai:
//    1) Single        : A -> B
//    2) Multilevel    : A -> B -> C
//    3) Multiple      : (A, B) -> C   [do base classes]
//    4) Hierarchical  : A -> B, A -> C  [ek base, kai child]
//    5) Hybrid        : multiple + hierarchical ka mix
// ============================================================================
#include <iostream>
#include <string>

using namespace std;

// ---------------- SINGLE INHERITANCE ----------------
// Ek base, ek derived. Dog ko Animal ka sound() inherit me milta hai.
class Animal
{
public:
    void sound()
    {
        cout << "Animal makes sound" << endl;
    }
};

class Dog : public Animal // Dog IS-A Animal
{
public:
    void bark()
    {
        cout << "Dog barks" << endl;
    }
};

// ---------------- MULTILEVEL INHERITANCE ----------------
// Chain: Fruit -> Mango -> Alphonso. Alphonso ke paas teeno level ke members.
class Fruit
{
public:
    string name;
};

class Mango : public Fruit // Mango IS-A Fruit
{
public:
    int weight;
};

class Alphonso : public Mango // Alphonso IS-A Mango (-> IS-A Fruit bhi)
{
public:
    int sugarLevel;
};

// ---------------- MULTIPLE INHERITANCE ----------------
// Ek class do (ya zyada) base classes se inherit karti hai.
class A
{
public:
    int physics = 85;
};

class B
{
public:
    int chemistry = 90;
};

class C : public A, public B // C ke paas A aur B dono ke members
{
public:
    int maths = 95;
};

// ---------------- HIERARCHICAL INHERITANCE ----------------
// Ek hi base se kai child classes nikalti hain.
class Parent
{
public:
    void show()
    {
        cout << "This is parent class" << endl;
    }
};

class Child1 : public Parent
{
};

class Child2 : public Parent
{
};

// ---------------- HYBRID INHERITANCE ----------------
// Multiple + hierarchical ka combination. Result do base classes se inherit.
class Student
{
public:
    string name = "Rahul";
};

class Marks
{
public:
    int score = 88;
};

class Result : public Student, public Marks // Student + Marks dono se
{
public:
    void display()
    {
        cout << "Name: " << name << endl;   // Student se
        cout << "Score: " << score << endl; // Marks se
    }
};

// ---------------- MAIN FUNCTION ----------------
int main()
{
    // SINGLE — Dog ke paas apna bark() + inherited sound()
    cout << "\n--- Single Inheritance ---\n";
    Dog d;
    d.sound();
    d.bark();

    // MULTILEVEL — Alphonso ke paas Fruit + Mango + apne members
    cout << "\n--- Multilevel Inheritance ---\n";
    Alphonso a;
    a.name = "Alphonso Mango";
    a.weight = 300;
    a.sugarLevel = 90;

    cout << a.name << " " << a.weight << "g " << a.sugarLevel << "%\n";

    // MULTIPLE — C ke paas A.physics + B.chemistry + apna maths
    cout << "\n--- Multiple Inheritance ---\n";
    C obj;
    cout << obj.physics << " " << obj.chemistry << " " << obj.maths << endl;

    // HIERARCHICAL — dono child ek hi Parent::show() share karte hain
    cout << "\n--- Hierarchical Inheritance ---\n";
    Child1 c1;
    Child2 c2;
    c1.show();
    c2.show();

    // HYBRID — Result do base se mila ke display karta hai
    cout << "\n--- Hybrid Inheritance ---\n";
    Result r;
    r.display();

    return 0;
}
