#include <iostream>
using namespace std;

// ---------------- SINGLE INHERITANCE ----------------
class Animal
{
public:
    void sound()
    {
        cout << "Animal makes sound" << endl;
    }
};

class Dog : public Animal
{
public:
    void bark()
    {
        cout << "Dog barks" << endl;
    }
};

// ---------------- MULTILEVEL INHERITANCE ----------------
class Fruit
{
public:
    string name;
};

class Mango : public Fruit
{
public:
    int weight;
};

class Alphonso : public Mango
{
public:
    int sugarLevel;
};

// ---------------- MULTIPLE INHERITANCE ----------------
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

class C : public A, public B
{
public:
    int maths = 95;
};

// ---------------- HIERARCHICAL INHERITANCE ----------------
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

class Result : public Student, public Marks
{
public:
    void display()
    {
        cout << "Name: " << name << endl;
        cout << "Score: " << score << endl;
    }
};

// ---------------- MAIN FUNCTION ----------------
int main()
{
    // SINGLE
    cout << "\n--- Single Inheritance ---\n";
    Dog d;
    d.sound();
    d.bark();

    // MULTILEVEL
    cout << "\n--- Multilevel Inheritance ---\n";
    Alphonso a;
    a.name = "Alphonso Mango";
    a.weight = 300;
    a.sugarLevel = 90;

    cout << a.name << " " << a.weight << "g " << a.sugarLevel << "%\n";

    // MULTIPLE
    cout << "\n--- Multiple Inheritance ---\n";
    C obj;
    cout << obj.physics << " " << obj.chemistry << " " << obj.maths << endl;

    // HIERARCHICAL
    cout << "\n--- Hierarchical Inheritance ---\n";
    Child1 c1;
    Child2 c2;
    c1.show();
    c2.show();

    // HYBRID
    cout << "\n--- Hybrid Inheritance ---\n";
    Result r;
    r.display();

    return 0;
}