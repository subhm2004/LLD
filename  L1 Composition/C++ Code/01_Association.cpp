/**
 * ============================================================================
 *  01_Association.cpp  —  ASSOCIATION (object relationship #1)
 * ----------------------------------------------------------------------------
 *  Association = "ek object dusre ko JAANTA/USE karta hai, par uska MALIK nahi
 *                 hai. Dono ki lifetime alag — ek marne pe dusra zinda reh sakta."
 *
 *  Asli duniya ka example: Teacher aur Students 👨‍🏫
 *    Teacher students ko jaanta hai (unhe padhata hai), par students ko na
 *    banata hai na destroy karta. Teacher retire ho jaye to students zinda
 *    rehte hain (kisi aur teacher ke paas). Bas ek-dusre ko "know" karte hain.
 *
 *  ┌──────────────────────────────────────────────────────────────────────────┐
 *  │  4 OBJECT RELATIONSHIPS ka comparison (is folder ka poora topic):       │
 *  │  (weak se strong ki taraf — coupling badhta jaata hai)                  │
 *  │                                                                          │
 *  │  Relationship | Kya matlab        | Lifetime      | UML symbol          │
 *  │  -------------+-------------------+---------------+-------------------   │
 *  │  Dependency   | method me use     | temporary     | dashed arrow  ..>   │
 *  │  (04 file)    | (parameter)       | (call ke liye)|                     │
 *  │  Association  | field me knows    | INDEPENDENT   | plain arrow  -->    │
 *  │  (YE file)    | (uses, not owns)  | (alag-alag)   |                     │
 *  │  Aggregation  | weak has-a        | SHARED/       | hollow diamond ◇    │
 *  │  (02 file)    | (part reusable)   | external      |                     │
 *  │  Composition  | strong has-a      | EXCLUSIVE     | filled diamond ◆    │
 *  │  (03 file)    | (part owned)      | (saath jeete- |                     │
 *  │               |                   |  marte hain)  |                     │
 *  └──────────────────────────────────────────────────────────────────────────┘
 *
 *  📌 ASSOCIATION ki pehchaan (is file me):
 *  - Teacher ke paas Student* ka field hai (KNOWS them)
 *  - Par Teacher unhe `new` NAHI karti (banata nahi)
 *  - Aur destructor me `delete` NAHI karti (OWN nahi karta)
 *  - Students Teacher ke BAHAR bante hain, Teacher ke baad bhi zinda rehte
 *  Yahi association ko aggregation/composition se alag karta hai — ye "know"
 *  hai, "own" nahi. Raw pointer store hota par ownership kisi aur ki.
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
    // Students stack par bane — Teacher ke BAHAR, independent lifetime.
    // Inhe koi Teacher nahi bana raha — ye khud se exist karte hain.
    Student alice("Alice");
    Student bob("Bob");

    Teacher prof("Prof. Sharma");
    prof.enroll(&alice); // sirf ADDRESS pass kiya (banaya nahi, reference rakha)
    prof.enroll(&bob);
    prof.teach();

    // >>> ASSOCIATION KA PROOF <<<
    // Teacher ka kaam khatam, par students abhi bhi zinda hain — kyunki
    // Teacher unka MALIK nahi tha, bas jaanta tha. Agar composition hota to
    // Teacher ke marte hi students bhi mar jaate.
    cout << "Students still exist: " << alice.getName() << ", " << bob.getName() << "\n";
    return 0;
    // Note: alice/bob main ke end pe destroy honge (Teacher se independent).
}

/**
 * EXPECTED OUTPUT:
 *   [Association] Prof. Sharma enrolled Alice
 *   [Association] Prof. Sharma enrolled Bob
 *   [Association] Prof. Sharma teaching: Alice Bob
 *   Students still exist: Alice, Bob
 *
 *   ^ Yahi last line association ka saboot — Teacher ke kaam ke baad bhi
 *   students zinda. "Knows-a", not "owns-a".
 */
