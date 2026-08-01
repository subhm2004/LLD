// ============================================================================
//  00_Five_Types_Of_Inheritance.cpp  —  Inheritance ke 5 types (overview)
// ----------------------------------------------------------------------------
//  Inheritance = "IS-A" relationship; child parent ke members reuse karta hai.
//  5 types: Single (A->B), Multilevel (A->B->C), Multiple ((A,B)->C),
//  Hierarchical (A->B, A->C), Hybrid (multiple+hierarchical mix). Yeh file
//  sab types ek saath dikhati hai. (Detailed L4 me bhi hai.)
// ============================================================================
#include <iostream>
#include <string>

using namespace std;

/*
 * All 5 types of inheritance in ONE file (interview / revision)
 *
 * 1. Single       — one parent, one child
 * 2. Multilevel   — Base → Derived → GrandChild (chain)
 * 3. Multiple     — two+ parents, one child
 * 4. Hierarchical — one parent, many children
 * 5. Hybrid       — combination (here: multilevel + multiple)
 *
 * See also: L4 UML_Diagrams/inheritance.cpp (same 5 demos, different examples)
 */

// =============================================================================
// 1. SINGLE INHERITANCE — ek parent, ek child
// =============================================================================
class Animal {
public:
    void sound() const { cout << "  Animal makes sound\n"; }
};

class Dog : public Animal {
public:
    void bark() const { cout << "  Dog barks\n"; }
};

// =============================================================================
// 2. MULTILEVEL INHERITANCE — Base → Derived → GrandChild
// =============================================================================
class Fruit {
public:
    string name = "Fruit";
};

class Mango : public Fruit {
public:
    int weightGrams = 0;
};

class Alphonso : public Mango {
public:
    int sugarPercent = 0;
};

// =============================================================================
// 3. MULTIPLE INHERITANCE — do parents, ek child
// =============================================================================
class Printer {
public:
    void print() const { cout << "  Printing document...\n"; }
};

class Scanner {
public:
    void scan() const { cout << "  Scanning document...\n"; }
};

class AllInOne : public Printer, public Scanner {
public:
    void copy() const { cout << "  Copying (print + scan)...\n"; }
};

// =============================================================================
// 4. HIERARCHICAL INHERITANCE — ek parent, kai children
// =============================================================================
class Vehicle {
public:
    void start() const { cout << "  Vehicle started\n"; }
};

class Bike : public Vehicle {};

class Truck : public Vehicle {};

// =============================================================================
// 5. HYBRID INHERITANCE — 2+ types ka mix
//    Yahan: MULTILEVEL (Person → Employee) + MULTIPLE (Employee + Student → TA)
// =============================================================================
class Person {
public:
    string name = "Unknown";
};

class Employee : public Person {
public:
    string department = "Engineering";
};

class Student {
public:
    int rollNumber = 0;
};

// TA inherits Employee (multilevel from Person) AND Student (multiple)
class TeachingAssistant : public Employee, public Student {
public:
    void teach() const {
        cout << "  TA " << name << " (" << department << ") teaches roll#"
             << rollNumber << "\n";
    }
};

// =============================================================================
// main — har type ka chhota demo
// =============================================================================
int main() {
    cout << "========== 1. SINGLE INHERITANCE ==========\n";
    cout << "  Dog : Animal — ek parent, ek child\n";
    Dog dog;
    dog.sound();
    dog.bark();

    cout << "\n========== 2. MULTILEVEL INHERITANCE ==========\n";
    cout << "  Alphonso : Mango : Fruit\n";
    Alphonso mango;
    mango.name = "Alphonso";
    mango.weightGrams = 300;
    mango.sugarPercent = 22;
    cout << "  " << mango.name << " | " << mango.weightGrams << "g | "
         << mango.sugarPercent << "% sugar\n";

    cout << "\n========== 3. MULTIPLE INHERITANCE ==========\n";
    cout << "  AllInOne : Printer, Scanner\n";
    AllInOne machine;
    machine.print();
    machine.scan();
    machine.copy();

    cout << "\n========== 4. HIERARCHICAL INHERITANCE ==========\n";
    cout << "  Bike : Vehicle,  Truck : Vehicle — ek parent, do child\n";
    Bike bike;
    Truck truck;
    bike.start();
    truck.start();

    cout << "\n========== 5. HYBRID INHERITANCE ==========\n";
    cout << "  TA : Employee : Person  +  TA : Student  (multilevel + multiple)\n";
    TeachingAssistant ta;
    ta.name = "Priya";
    ta.department = "CS";
    ta.rollNumber = 101;
    ta.teach();

    cout << "\n[Done] All 5 inheritance types demonstrated.\n";
    return 0;
}
