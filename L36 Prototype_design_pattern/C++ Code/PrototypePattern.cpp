// ============================================================================
//  PrototypePattern.cpp  —  Prototype Design Pattern (GoF Creational)
// ----------------------------------------------------------------------------
//  BHAI SIMPLE FUNDA:
//  Naya object SCRATCH se banane (heavy constructor chalane) ki jagah,
//  ek already-bana hua TEMPLATE object CLONE kar lo!
//
//  GoF definition: "Specify the kinds of objects to create using a
//  prototypical instance, and create new objects by COPYING this prototype."
//
//  WithoutPrototype.cpp ki problem yaad karo:
//    - Har NPC pe heavy constructor (DB call + calc) dobara chalta tha
//    - Saari stats har baar manually pass karni padti thi
//
//  PROTOTYPE ka solution:
//    1. Mehnga setup sirf EK baar karo — template/prototype object banao
//    2. Uske baad jitne objects chahiye — template.clone() — sasta copy!
//    3. Clone ko thoda tweak karna ho (naam, health) to setters se kar lo
//
//  Analogy: Photocopy machine 📄 — original document ek baar mehnat se
//  banaya (likha, design kiya). Ab 100 copies chahiye to 100 baar likhoge?
//  Nahi! Photocopy karo — fast aur galti-proof. Original = prototype,
//  photocopy = clone(), copy pe naam badal dena = setter tweak.
//
//  Ek aur analogy: Biology wali cell division 🧬 — cell apne aap ko
//  duplicate karti hai, har baar zero se nahi banti!
//
//  C++ ME CLONE KAISE: clone() andar COPY CONSTRUCTOR use hota hai —
//  new NPC(*this). Copy constructor me heavy kaam (DB/calc) NAHI hota,
//  bas fields copy hote hain. Isi liye clone sasta hai.
//
//  ⚠️ DEEP vs SHALLOW COPY: Agar class me pointer/dynamic memory ho to
//  copy constructor me DEEP COPY karna zaroori hai — warna dono objects
//  ek hi memory share karenge (double delete ka khatra!). Yahan sab
//  value-type fields hain (string, int) to default jaisa copy safe hai.
// ============================================================================
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ============================================================================
//  SECTION 1: PROTOTYPE INTERFACE — Cloneable
// ----------------------------------------------------------------------------
//  Har wo class jo apne aap ko clone karna support kare, ye interface
//  implement karegi. Client sirf clone() jaanta hai — concrete class
//  ka constructor use karne ki zaroorat hi nahi!
// ============================================================================
class Cloneable {
public:
    // Pure virtual clone — har cloneable type apni copy banana khud jaane.
    // 'const' hai kyunki clone karne se ORIGINAL nahi badalna chahiye.
    virtual Cloneable* clone() const = 0;

    // Virtual destructor — base pointer (Cloneable*) se derived object
    // delete hoga, isliye zaroori (warna derived ka dtor skip → leak/UB).
    virtual ~Cloneable() {}
};

// ============================================================================
//  SECTION 2: CONCRETE PROTOTYPE — NPC
// ----------------------------------------------------------------------------
//  Game ka NPC jo apne aap ko clone kar sakta hai.
// ============================================================================
class NPC : public Cloneable {
public:
    string name;
    int health;
    int attack;
    int defense;

    // -------------------- HEAVY CONSTRUCTOR --------------------
    // Wahi mehnga constructor (DB call + complex calc) — PAR ab ye
    // sirf TEMPLATE banane ke liye EK BAAR chalega. Clones is raste
    // se nahi aayenge!
    NPC(const string& name, int health, int attack, int defense) {
        // call database        ← mehnga kaam — sirf template ke liye
        // complex calc         ← mehnga kaam — sirf template ke liye
        this->name = name;
        this->health = health;
        this->attack = attack;
        this->defense = defense;
        cout << "Setting up template NPC '" << name << "'\n";
    }

    // -------------------- COPY CONSTRUCTOR --------------------
    // YAHI clone ka engine hai! Dhyan do — isme DB call / calc NAHI hai,
    // bas fields ki seedhi copy. Isi liye clone karna SASTA hai.
    // (Agar pointer members hote to yahan DEEP COPY likhni padti —
    //  nayi memory allocate karke content copy karna.)
    NPC(const NPC& other) {
        name = other.name;
        health = other.health;
        attack = other.attack;
        defense = other.defense;
        cout << "Cloning NPC '" << name << "'\n";
    }

    // -------------------- PROTOTYPE KA CORE: clone() --------------------
    // "Apni copy khud bana ke de do" — object khud jaanta hai wo kaun hai,
    // isliye copy banane ke liye CLIENT ko concrete class jaanne ki
    // zaroorat nahi. new NPC(*this) → copy constructor chalega
    // (heavy constructor NAHI!).
    //
    // NOTE: Return type Cloneable* hai (BASE pointer) — standard GoF
    // style, kyunki interface me yahi likha hai. Iska matlab: caller ko
    // NPC ke special methods (setName, describe...) chahiye to use
    // dynamic_cast se NPC* me DOWNCAST karna padega.
    // (dynamic_cast ki puri detail ke liye dekho: dynamic_cast.cpp)
    Cloneable* clone() const override {
        return new NPC(*this); // copy-ctor → DB/calc dobara nahi chalega
    }

    void describe() {
        cout << "NPC " << name  << " [HP=" << health  << " ATK=" << attack
             << " DEF=" << defense << "]\n";
    }

    // -------------------- SETTERS --------------------
    // Clone banane ke baad customize karne ke liye — "photocopy pe
    // naam badal do" wala kaam. Clone + tweak = naya variant, bina
    // heavy setup ke!
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

// ============================================================================
//  SECTION 3: CLIENT CODE — main()
// ----------------------------------------------------------------------------
//  Flow: 1 template (mehnga) → N clones (saste) → tweak as needed
// ============================================================================
int main() {
    // STEP 1: Template banao — heavy constructor SIRF YAHAN chalega.
    // Output me "Setting up template..." sirf EK baar dikhega.
    NPC* alien = new NPC("Alien", 30, 5, 2);

    // STEP 2: Clone #1 — bilkul same alien chahiye tha, ek line me mil gaya!
    // Na stats pass ki, na DB call hua — copy constructor ne fields copy kiye.
    //
    // dynamic_cast KYUN? clone() ka return type Cloneable* hai (BASE
    // pointer), par humein NPC* chahiye — describe()/setters sirf NPC
    // ke paas hain, Cloneable ke paas nahi. To base → derived DOWNCAST
    // karna padega, aur uska SAFE tareeka hai dynamic_cast:
    //   - andar sach me NPC hai  → valid NPC* milega ✅
    //   - andar NPC nahi hota    → nullptr milta (crash nahi!)
    // Yahan hum pakka jaante hain ki NPC hi hai, isliye seedha use kiya.
    // (dynamic_cast ki full detail: dynamic_cast.cpp file dekho!)
    NPC* alienCopied1 = dynamic_cast<NPC*>(alien->clone());
    alienCopied1->describe();

    // STEP 3: Clone #2 + TWEAK — "Powerful Alien" chahiye jo template
    // jaisa hi hai bas naam aur health alag. Clone karo, 2 setters
    // chalao — ho gaya! WithoutPrototype me iske liye saari 4 stats
    // phir se pass karni padti thi + heavy setup chalta tha.
    NPC* alienCopied2 = dynamic_cast<NPC*>(alien->clone());
    alienCopied2->setName("Powerful Alien"); // sirf jo badalna hai wahi badlo
    alienCopied2->setHealth(50);
    alienCopied2->describe();

    // 100 aliens chahiye? Loop me alien->clone() — 100 saste copies,
    // DB call phir bhi sirf 1 (template wala)! 

    // Cleanup — teeno heap objects delete (Cloneable ka virtual dtor
    // hone se base pointer se bhi safe delete hota).
    delete alien;
    delete alienCopied1;
    delete alienCopied2;
}

