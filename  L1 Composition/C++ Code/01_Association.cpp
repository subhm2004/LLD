/**
 * ASSOCIATION — loosely connected; no ownership
 * Teacher uses Student; both can exist independently
 * UML: simple arrow  -->  (no diamond)
 */
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Student {
    string name;
public:
    Student(string n) : name(n) {}
    string getName() const { return name; }
};

class Teacher {
    string name;
    // Association: Teacher "knows" students but does NOT own them
    vector<Student*> studentsEnrolled;

public:
    Teacher(string n) : name(n) {}

    void enroll(Student* s) {
        studentsEnrolled.push_back(s);
        cout << "[Association] " << name << " enrolled " << s->getName() << "\n";
    }

    void teach() const {
        cout << "[Association] " << name << " teaching: ";
        for (Student* s : studentsEnrolled)
            cout << s->getName() << " ";
        cout << "\n";
    }
};

int main() {
    Student alice("Alice");
    Student bob("Bob");

    Teacher prof("Prof. Sharma");
    prof.enroll(&alice);
    prof.enroll(&bob);
    prof.teach();

  // Students exist after teacher scope — independent lifetimes
    cout << "Students still exist: " << alice.getName() << ", " << bob.getName() << "\n";
    return 0;
}
