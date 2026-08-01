// ============================================================================
//  WithoutPrototype.cpp  —  Prototype ke BINA (problem samajhne ke liye)
// ----------------------------------------------------------------------------
//  BHAI SIMPLE FUNDA:
//  Ye file jaan-bujh kar GALAT (mehnga) tareeka dikhati hai, taaki
//  PrototypePattern.cpp ka fix samajh me aaye. Pehle problem feel karo!
//
//  SCENARIO: Game me NPCs (Non-Player Characters) banane hain — aliens,
//  monsters wagera. NPC banana MEHNGA kaam hai:
//    - Database se stats/config load karna
//    - Complex calculations (level scaling, AI setup...)
//
//  PROBLEM YAHAN KYA HAI:
//  1. Har naye NPC ke liye HEAVY CONSTRUCTOR poora dobara chalta hai —
//     wahi DB call, wahi calculation, bar-bar. Waste!
//  2. Client ko SAARI stats manually pass karni padti hain — 4 fields
//     yaad rakho, sahi order me do. Ek galti = bugged enemy.
//  3. Do NPCs me sirf EK field ka farq hai (defense 2 vs 5), phir bhi
//     poora setup zero se karna pada — "thoda sa alag" ka koi shortcut nahi.
//  4. 100 similar aliens chahiye? 100 baar heavy setup + 100 jagah
//     stats copy-paste karne ka risk.
//
//  FIX (agli file me): Ek template NPC EK BAAR banao (heavy kaam ek baar),
//  phir usse clone() karke saste copies lo — sirf jo badalna hai wo tweak karo.
// ============================================================================
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ============================================================================
//  NPC CLASS — bilkul seedhi-saadhi, koi Prototype/clone facility NAHI
// ============================================================================
class NPC {
public:
    // Public fields — demo simple rakhne ke liye (production me private + getters)
    string name;
    int health;
    int attack;
    int defense;

    // -------------------- "HEAVY" CONSTRUCTOR --------------------
    // Yahi is file ka villain hai! Socho is constructor ke andar:
    //   - Database call hota hai (stats/config fetch karne ke liye)
    //   - Complex calculation hota hai (balancing, AI init...)
    // Aur ye PURA KAAM har ek NPC banate waqt DOBARA hota hai —
    // chahe naya NPC purane se 99% same hi kyun na ho!
    NPC(const string& name, int health, int attack, int defense) {

        // call database        ← imagine: 200ms ka network call
        // complex calc         ← imagine: bhaari computation
        this->name = name;
        this->health = health;
        this->attack = attack;
        this->defense = defense;

        cout << "Creating NPC '" << name << "' [HP:" << health << ", ATK:"
             << attack << ", DEF:" << defense << "]\n";
    }

    // Apni stats print karne ka helper
    void describe() {
        cout << "  NPC: " << name << " | HP=" << health << " ATK=" << attack
             << " DEF=" << defense << "\n";
    }
};

// ============================================================================
//  CLIENT CODE — yahan problem clearly dikhti hai
// ============================================================================
int main() {
    // NPC #1: Base Alien — heavy constructor chala (DB + calc). Thik hai,
    // pehli baar to banana hi tha.
    NPC* alien = new NPC("Alien", 30, 5, 2);
    alien->describe();

    // NPC #2: Powerful Alien — DHYAN SE DEKHO:
    //   - Sirf name aur defense alag hai (30, 5 wahi ke wahi)
    //   - Phir bhi SAARI stats manually pass karni padi
    //   - Aur heavy constructor (DB + calc) PHIR SE poora chala!
    // Agar galti se (30, 5, 5) ki jagah (3, 5, 5) likh diya to?
    // Ek-hit-kill enemy ban jayega — silent bug, compiler nahi pakdega.
    NPC* alien2 = new NPC("Powerful Alien", 30, 5, 5);
    alien2->describe();

    // Ab socho: level me 100 aliens chahiye...
    // for (int i = 0; i < 100; i++)
    //     new NPC("Alien", 30, 5, 2);   ← 100 DB calls! 100 baar stats likho!
    // Yahi pain Prototype pattern solve karta hai → PrototypePattern.cpp dekho.

    // Cleanup — jo new kiya wo delete karo
    delete alien;
    delete alien2;
    return 0;
}

// ============================================================================
//  EXPECTED OUTPUT:
// ----------------------------------------------------------------------------
//  Creating NPC 'Alien' [HP:30, ATK:5, DEF:2]
//    NPC: Alien | HP=30 ATK=5 DEF=2
//  Creating NPC 'Powerful Alien' [HP:30, ATK:5, DEF:5]
//    NPC: Powerful Alien | HP=30 ATK=5 DEF=5
//
//  Dono baar "Creating NPC" — matlab heavy setup DONO baar chala. ❌
//  (Prototype me dusri baar "Cloning" dikhega — sasta kaam. ✅)
// ============================================================================
