/**
 * ============================================================================
 *  01_Association.cpp  —  ASSOCIATION (Has-A ka sabse weak structural link)
 * ----------------------------------------------------------------------------
 *  Association = ek object dusre ko "jaanta/use karta" hai, par MALIK nahi hai.
 *  Dono ki lifetime alag-alag — ek delete ho to dusra zinda reh sakta hai.
 *  Yahan Teacher students ko jaanta hai (field me rakhta hai) par unhe na
 *  banata hai na destroy karta. UML: simple arrow  -->  (koi diamond nahi).
 * ============================================================================
 */
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Student ek independent entity hai — Teacher se alag jee sakta hai.
class Student {
    string name;
public:
    Student(string n) : name(n) {}
    string getName() const { return name; }
};

class Teacher {
    string name;
    // Association: Teacher students ko "jaanta" hai par OWN nahi karta.
    // Yeh raw pointers hain jinhe Teacher delete NAHI karega (ownership nahi).
    vector<Student*> studentsEnrolled;

public:
    Teacher(string n) : name(n) {}

    // Student ka address store karte hain — sirf reference rakha, banaya nahi.
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
    // Note: Teacher ke destructor me students delete NAHI hote — yahi association.
};

int main() {
    // Students stack par bane — Teacher ke bahar, independent lifetime.
    Student alice("Alice");
    Student bob("Bob");

    Teacher prof("Prof. Sharma");
    prof.enroll(&alice); // sirf address pass kiya
    prof.enroll(&bob);
    prof.teach();

    // Teacher ka kaam khatam hone par bhi students zinda hain — proof of association.
    cout << "Students still exist: " << alice.getName() << ", " << bob.getName() << "\n";
    return 0;
}
