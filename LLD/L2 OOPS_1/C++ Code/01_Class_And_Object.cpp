// ============================================================================
//  01_Class_And_Object.cpp  —  Class aur Object ke basics (OOP foundation)
// ----------------------------------------------------------------------------
//  Class = ek BLUEPRINT (design) — batata hai ki data (fields) aur behavior
//  (methods) kya honge. Object = us blueprint ka ASLI instance (memory me bana
//  hua). Ek class se kai objects ban sakte hain. Yahan Student class hai, aur
//  do objects: ek stack par (s1), ek heap par (s2 via new).
// ============================================================================
#include <iostream>
#include <string>
using namespace std;

class Student {
    // private data members — bahar se directly access nahi (encapsulation)
    string name;
    int rollNo;

public:
    // Constructor — object bante hi fields initialize karta hai
    Student(string n, int r) : name(n), rollNo(r) {}

    void display() const {
        cout << "Student: " << name << " (Roll " << rollNo << ")\n";
    }
};

int main() {
    // Stack object — scope khatam hote hi automatic destroy
    Student s1("Aditya", 101);
    s1.display();

    // Heap object — `new` se bana, isliye `delete` khud karna padega
    Student* s2 = new Student("Rohit", 102);
    s2->display();
    delete s2; // warna memory leak

    return 0;
}
