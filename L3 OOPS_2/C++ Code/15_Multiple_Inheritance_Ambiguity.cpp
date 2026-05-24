#include <iostream>
using namespace std;

// --- Ambiguity type 1: two bases with same member name ---
class Printer {
public:
    void print() { cout << "  Printer::print\n"; }
};

class Scanner {
public:
    void print() { cout << "  Scanner::print\n"; }
};

class AllInOne : public Printer, public Scanner {
public:
    void demoScope() {
        Printer::print();   // fix: qualify which base
        Scanner::print();
    }
};

// --- Ambiguity type 2: diamond — two paths to same base ---
class Person {
public:
    string name = "Unknown";
    void introduce() { cout << "  Person: " << name << endl; }
};

class Student : public Person {};
class Employee : public Person {};

class TeachingAssistant : public Student, public Employee {
public:
    void showAmbiguity() {
        // name = "TA";           // ERROR: ambiguous — Student::Person or Employee::Person?
        // introduce();           // ERROR: which Person subobject?
        cout << "  TA has TWO Person subobjects (diamond)\n";
        cout << "  Student::name = " << Student::name << endl;
        cout << "  Employee::name = " << Employee::name << endl;
        Student::name = "Alice (student side)";
        Employee::name = "Alice (employee side)";  // can be different strings!
        Student::introduce();
        Employee::introduce();
    }
};

// --- Fix: virtual inheritance — single shared Person ---
class PersonV {
public:
    string name = "Unknown";
    virtual void introduce() { cout << "  PersonV: " << name << endl; }
    virtual ~PersonV() = default;
};

class StudentV : public virtual PersonV {};
class EmployeeV : public virtual PersonV {};

class TAVirtual : public StudentV, public EmployeeV {
public:
    void assignName(const string& n) {
        name = n;              // OK — one PersonV
        introduce();
    }
};

int main() {
    cout << "=== MI ambiguity: same method in two direct bases ===\n";
    AllInOne device;
    // device.print();  // ERROR if uncommented
    device.demoScope();

    cout << "\n=== Diamond ambiguity: duplicate base subobjects ===\n";
    TeachingAssistant ta;
    ta.showAmbiguity();
    cout << "  sizeof(TeachingAssistant) = " << sizeof(TeachingAssistant) << endl;

    cout << "\n=== Virtual inheritance: one shared base ===\n";
    TAVirtual ta2;
    ta2.assignName("Bob");
    cout << "  sizeof(TAVirtual) = " << sizeof(TAVirtual) << endl;

    cout << "\nAlso see: 08_Diamond_Problem.cpp for virtual base + sizeof demo.\n";
    return 0;
}
