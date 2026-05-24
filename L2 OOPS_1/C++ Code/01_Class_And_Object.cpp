/**
 * L2 — Class & Object basics
 * Class = blueprint | Object = runtime instance
 */
#include <iostream>
#include <string>
using namespace std;

class Student {
    string name;
    int rollNo;

public:
    Student(string n, int r) : name(n), rollNo(r) {}

    void display() const {
        cout << "Student: " << name << " (Roll " << rollNo << ")\n";
    }
};

int main() {
    // Stack object
    Student s1("Aditya", 101);
    s1.display();

    // Heap object
    Student* s2 = new Student("Rohit", 102);
    s2->display();
    delete s2;

    return 0;
}
