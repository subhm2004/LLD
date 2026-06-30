// ============================================================================
//  WithoutPrototype.cpp  —  Prototype ke BINA (problem dikhane ke liye)
// ----------------------------------------------------------------------------
//  Yahan har naya NPB banane ke liye heavy constructor (DB + calc) DOBARA chalta
//  hai, aur saari stats manually re-pass karni padti hain. 100 similar aliens
//  chahiye? 100 baar yahi mehnga kaam + galti ka risk. Iska fix Prototype
//  pattern (PrototypePattern.cpp) — ek template clone karo.
// ============================================================================
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// Simple NPC class — koi Prototype/clone nahi.
class NPC {
public:
    string name;
    int health;
    int attack;
    int defense;

    // "Heavy" constructor: har field deni padti hai, aur DB/calc har baar chalta hai.
    NPC(const string& name, int health, int attack, int defense) {

        // call database
        // complex calc
        this->name = name;
        this->health = health;
        this->attack = attack;
        this->defense = defense;

        cout << "Creating NPC '" << name << "' [HP:" << health << ", ATK:"
             << attack << ", DEF:" << defense << "]\n";
    }

    void describe() {
        cout << "  NPC: " << name << " | HP=" << health << " ATK=" << attack
             << " DEF=" << defense << "\n";
    }
};

int main() {
    // Base Alien — heavy constructor chala.
    NPC* alien = new NPC("Alien", 30, 5, 2);
    alien->describe();

    // Powerful Alien — saari stats phir se manually deni padi (galti easy).
    NPC* alien2 = new NPC("Powerful Alien", 30, 5, 5);
    alien2->describe();

    // 100 aliens chahiye to yeh 100 baar repeat hoga (har baar heavy setup).

    // cleanup
    delete alien;
    delete alien2;
    return 0;
}
