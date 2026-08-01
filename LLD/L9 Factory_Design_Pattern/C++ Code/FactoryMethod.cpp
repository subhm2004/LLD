// ============================================================================
//  FactoryMethod.cpp  —  Factory Method Pattern (GoF Creational)
// ----------------------------------------------------------------------------
//  BHAI SIMPLE FUNDA:
//  SimpleFactory.cpp me EK concrete factory thi jisme saara if-else tha.
//  Problem: naya brand/variant aaye to USI class ko edit karna padta (OCP break).
//
//  FACTORY METHOD ka solution:
//  Factory ko hi ABSTRACT bana do! Ek interface (BurgerFactory) jo bas
//  declare karta hai — "createBurger() hoga". Ab HAR BRAND apni concrete
//  factory class banata hai:
//    - SinghBurger factory → normal bun wale burgers banati hai
//    - KingBurger factory  → wheat bun wale burgers banati hai
//
//  GoF definition: "Define an interface for creating an object, but let
//  SUBCLASSES decide which class to instantiate."
//  Matlab — object banane ka faisla SUBCLASS pe chhod do.
//
//  FAYDA (Simple Factory se behtar):
//  - Naya brand chahiye? → Nayi factory class banao (McDBurger). Purana
//    code ZERO edit — Open/Closed Principle satisfy!
//  - Client sirf abstract BurgerFactory* ke saath kaam karta hai —
//    use pata hi nahi kaunsa brand serve kar raha hai (DIP).
//
//  Analogy: Franchise model — "Singh Burger" aur "King Burger" dono ka
//  menu API same hai (createBurger), par har outlet apni recipe se
//  banata hai. Naya franchise kholna ho to purane outlets nahi badalte!
// ============================================================================
#include <bits/stdc++.h>

using namespace std;

// ============================================================================
//  SECTION 1: PRODUCT INTERFACE + CONCRETE PRODUCTS
// ----------------------------------------------------------------------------
//  Product side bilkul SimpleFactory jaisa hi hai — bas ab products ki
//  DO LINES hain: normal bun (Singh ke liye) aur wheat bun (King ke liye).
// ============================================================================

// --- Product interface: saare burgers ka common contract ---
class Burger
{
public:
    virtual void prepare() = 0; // pure virtual — har burger apna prepare() de
    virtual ~Burger() {}        // virtual destructor — base pointer se delete safe
};

// ---------------- Normal bun product line (SinghBurger banayega) ----------------

class BasicBurger : public Burger
{
public:
    void prepare() override
    {
        cout << "Preparing Basic Burger with bun, patty, and ketchup!" << endl;
    }
};

class StandardBurger : public Burger
{
public:
    void prepare() override
    {
        cout << "Preparing Standard Burger with bun, patty, cheese, and lettuce!" << endl;
    }
};

class PremiumBurger : public Burger
{
public:
    void prepare() override
    {
        cout << "Preparing Premium Burger with gourmet bun, premium patty, cheese, lettuce, and secret sauce!" << endl;
    }
};

// ---------------- Wheat bun product line (KingBurger banayega) ----------------
// Dhyan do: ye bhi Burger interface hi implement karte hain — client ke
// liye normal vs wheat ka koi farq nahi, sab Burger* hain.

class BasicWheatBurger : public Burger
{
public:
    void prepare() override
    {
        cout << "Preparing Basic Wheat Burger with bun, patty, and ketchup!" << endl;
    }
};

class StandardWheatBurger : public Burger
{
public:
    void prepare() override
    {
        cout << "Preparing Standard Wheat Burger with bun, patty, cheese, and lettuce!" << endl;
    }
};

class PremiumWheatBurger : public Burger
{
public:
    void prepare() override
    {
        cout << "Preparing Premium Wheat Burger with gourmet bun, premium patty, cheese, lettuce, and secret sauce!" << endl;
    }
};

// ============================================================================
//  SECTION 2: ABSTRACT CREATOR (yahi Factory Method ka core hai!)
// ----------------------------------------------------------------------------
//  SimpleFactory me factory CONCRETE class thi. Yahan factory khud
//  ABSTRACT hai — createBurger() pure virtual hai. Kaunsa burger banega,
//  ye decision SUBCLASS (concrete factory) leti hai.
//  Isi virtual createBurger() ko "FACTORY METHOD" kehte hain —
//  pattern ka naam isi method se aaya hai.
// ============================================================================
class BurgerFactory
{
public:
    // Factory Method — subclass isse override karke decide karegi
    // kaunsa concrete burger banana hai.
    virtual Burger *createBurger(string &type) = 0;

    // Virtual destructor — client BurgerFactory* (base pointer) se
    // SinghBurger/KingBurger delete karega, isliye zaroori.
    virtual ~BurgerFactory() {}
};

// ============================================================================
//  SECTION 3: CONCRETE CREATORS (brands)
// ----------------------------------------------------------------------------
//  Har brand ki apni factory — apna if-else, apni product line.
//  Naya brand aaye to bas ek aur aisi class — PURANA CODE UNTOUCHED (OCP!).
// ============================================================================

// --- Concrete Factory #1: Singh Burger — normal bun specialist ---
class SinghBurger : public BurgerFactory
{
public:
    Burger *createBurger(string &type) override
    {
        // Ye if-else ab bhi hai, PAR ab ye sirf IS brand ke variants ke
        // liye hai — saare brands ka logic ek jagah nahi thusa hua.
        if (type == "basic")
        {
            return new BasicBurger();
        }
        else if (type == "standard")
        {
            return new StandardBurger();
        }
        else if (type == "premium")
        {
            return new PremiumBurger();
        }
        else
        {
            cout << "Invalid burger type! " << endl;
            return nullptr;
        }
    }
};

// --- Concrete Factory #2: King Burger — wheat bun specialist ---
class KingBurger : public BurgerFactory
{
public:
    Burger *createBurger(string &type) override
    {
        // SAME type strings ("basic"/"standard"/"premium"), par ye factory
        // WHEAT versions banati hai — client ko farq pata bhi nahi chalta!
        if (type == "basic")
        {
            return new BasicWheatBurger();
        }
        else if (type == "standard")
        {
            return new StandardWheatBurger();
        }
        else if (type == "premium")
        {
            return new PremiumWheatBurger();
        }
        else
        {
            cout << "Invalid burger type! " << endl;
            return nullptr;
        }
    }
};

// ============================================================================
//  SECTION 4: CLIENT CODE — main()
// ----------------------------------------------------------------------------
//  Client ka SIRF EK decision: kaunsi factory (brand) use karni hai.
//  Uske baad sab kuch abstract interfaces se hota hai.
// ============================================================================
int main()
{
    string type = "basic";

    // Brand yahan choose hua — bas YE line badlo (new KingBurger())
    // aur pura output wheat versions me switch ho jayega. Baaki code same!
    // Yahi runtime polymorphism ka power hai.
    BurgerFactory *myFactory = new SinghBurger();

    // Client abstract factory se maang raha hai — use nahi pata andar
    // SinghBurger hai ya KingBurger. Virtual dispatch sahi createBurger()
    // chala dega.
    Burger *burger = myFactory->createBurger(type);

    burger->prepare(); // → BasicBurger::prepare() (kyunki Singh factory thi)

    // Cleanup — dono virtual destructors ki wajah se safe delete.
    delete burger;
    delete myFactory;

    return 0;
}

// ============================================================================
//  EXPECTED OUTPUT (SinghBurger factory, type = "basic"):
// ----------------------------------------------------------------------------
//  Preparing Basic Burger with bun, patty, and ketchup!
//
//  Agar factory KingBurger hoti to:
//  Preparing Basic Wheat Burger with bun, patty, and ketchup!
// ============================================================================
