// ============================================================================
//  PrototypePattern.cpp  —  Prototype Design Pattern (Creational)
// ----------------------------------------------------------------------------
//  Prototype = naya object banane ke liye ek already-bana TEMPLATE ko clone()
//  karo, scratch se constructor mat chalao. Faayda: agar setup mehnga hai
//  (DB call, heavy calculation), to woh sirf EK baar template par hota hai;
//  phir clone() se saste copies banते hain. Yahan game NPC ka template ek baar
//  banta hai, fir har enemy uska clone hai (+ thoda tweak).
//  (Clone = copy constructor; pointer/graph fields ho to deep copy ka dhyaan.)
// ============================================================================
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Prototype interface — har cloneable type clone() dega.
class Cloneable {
public:
    virtual Cloneable* clone() const = 0;
    virtual ~Cloneable() {}
};

class NPC : public Cloneable {
public:
    string name;
    int health;
    int attack;
    int defense;

    // "Heavy" constructor — yahan DB call / complex calc hota (sirf template ke liye).
    NPC(const string& name, int health, int attack, int defense) {
        // call database
        // complex calc
        this->name = name;
        this->health = health;
        this->attack = attack;
        this->defense = defense;
        cout << "Setting up template NPC '" << name << "'\n";
    }

    // Copy constructor — clone() isi ko use karta hai (heavy work dobara nahi).
    NPC(const NPC& other) {
        name = other.name;
        health = other.health;
        attack = other.attack;
        defense = other.defense;
        cout << "Cloning NPC '" << name << "'\n";
    }

    // Prototype ka core: apne aap ki copy bana ke return karo.
    Cloneable* clone() const override {
        return new NPC(*this); // copy-ctor -> no DB/calc rerun
    }

    void describe() {
        cout << "NPC " << name  << " [HP=" << health  << " ATK=" << attack
             << " DEF=" << defense << "]\n";
    }

    // Setters — clone ko thoda customize karne ke liye.
    void setName(const string& n) {
        name = n;
    }
    void setHealth(int h) {
        health = h;
    }
    void setAttack(int a) {
         attack = a;
    }
    void setDefense(int d){
        defense = d;
    }
};

int main() {
    // 1) Ek "heavy" template banao (mehnga setup ek hi baar).
    NPC* alien = new NPC("Alien", 30, 5, 2);

    // 2) Ab jitne chaaho saste clones banao + tweak karo (heavy setup nahi chalega).
    NPC* alienCopied1 = dynamic_cast<NPC*>(alien->clone());
    alienCopied1->describe();

    NPC* alienCopied2 = dynamic_cast<NPC*>(alien->clone());
    alienCopied2->setName("Powerful Alien"); // clone ko customize
    alienCopied2->setHealth(50);
    alienCopied2->describe();

    // cleanup
    delete alien;
    delete alienCopied1;
    delete alienCopied2;
}
