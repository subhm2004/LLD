// ============================================================================
//  AbstractFactory.cpp  —  Abstract Factory Pattern (GoF Creational)
// ----------------------------------------------------------------------------
//  BHAI SIMPLE FUNDA:
//  FactoryMethod.cpp me har factory sirf EK product line banati thi (Burger).
//  Par real life me COMBO chahiye hota hai — Burger KE SAATH GarlicBread!
//
//  Aur yahan ek naya problem aata hai — CONSISTENCY:
//  Agar burger wheat wala hai to garlic bread bhi WHEAT hi hona chahiye.
//  Do alag-alag factories use karo to galti se mix ho sakta hai:
//  wheat burger + normal bread = theme toot gayi! ❌
//
//  ABSTRACT FACTORY ka solution:
//  Ek factory interface jisme MULTIPLE create methods hain —
//  createBurger() AUR createGarlicBread(). Ab ek concrete factory
//  ek POORI RELATED FAMILY banati hai:
//    - SinghBurger factory → normal burger + normal garlic bread
//    - KingBurger factory  → wheat burger  + wheat garlic bread
//
//  GoF definition: "Provide an interface for creating FAMILIES of related
//  or dependent objects without specifying their concrete classes."
//
//  GUARANTEE: Ek factory se jo bhi loge, sab SAME THEME ka hoga —
//  mixing structurally impossible ho jaati hai (jab tak ek hi factory use karo).
//
//  FACTORY METHOD vs ABSTRACT FACTORY (interview ka favourite):
//  - Factory Method  → EK product banata hai (createBurger)
//  - Abstract Factory → PRODUCT FAMILY banata hai (createBurger + createGarlicBread)
//
//  Analogy: Meal combo — "wheat theme meal" bolo to burger aur bread
//  DONO wheat family se aayenge. Jaise UI me Dark Mode — button, checkbox,
//  menu SAB dark theme ke, koi light wala ghus nahi sakta.
// ============================================================================
#include <bits/stdc++.h>

using namespace std;

// ============================================================================
//  SECTION 1: PRODUCT #1 — BURGER (interface + dono families)
// ----------------------------------------------------------------------------
//  Abstract Factory me MULTIPLE product types hote hain. Ye pehla hai.
// ============================================================================

// --- Product interface #1 ---
class Burger
{
public:
    virtual void prepare() = 0; // pure virtual — har burger apna prepare()
    virtual ~Burger() {}        // virtual destructor — base pointer se delete safe
};

// ---------------- Normal bun family (Singh theme) ----------------

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

// ---------------- Wheat bun family (King theme) ----------------

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
//  SECTION 2: PRODUCT #2 — GARLIC BREAD (interface + dono families)
// ----------------------------------------------------------------------------
//  YAHI Abstract Factory ko Factory Method se alag karta hai —
//  DUSRA related product type! Iske bina ye bas Factory Method hota.
// ============================================================================

// --- Product interface #2 ---
class GarlicBread
{
public:
    virtual void prepare() = 0;
    virtual ~GarlicBread() {} // virtual destructor — same reason
};

// ---------------- Normal bread family (Singh theme) ----------------

class BasicGarlicBread : public GarlicBread
{
public:
    void prepare() override
    {
        std::cout << "Preparing Basic Garlic Bread with butter and garlic!\n";
    }
};

class CheeseGarlicBread : public GarlicBread
{
public:
    void prepare() override
    {
        std::cout << "Preparing Cheese Garlic Bread with extra cheese and butter!\n";
    }
};

// ---------------- Wheat bread family (King theme) ----------------

class BasicWheatGarlicBread : public GarlicBread
{
public:
    void prepare() override
    {
        std::cout << "Preparing Basic Wheat Garlic Bread with butter and garlic!\n";
    }
};

class CheeseWheatGarlicBread : public GarlicBread
{
public:
    void prepare() override
    {
        std::cout << "Preparing Cheese Wheat Garlic Bread with extra cheese and butter!\n";
    }
};

// ============================================================================
//  SECTION 3: ABSTRACT FACTORY — MealFactory
// ----------------------------------------------------------------------------
//  Pattern ka DIL. Dhyan do — isme DO pure virtual create methods hain
//  (Factory Method me sirf ek tha). Jo bhi concrete factory banegi,
//  use DONO products banane padenge — aur wo apni family ke hi banayegi.
//  Isi se family consistency milti hai.
// ============================================================================
class MealFactory
{
public:
    virtual Burger *createBurger(string &type) = 0;           // product #1 ka factory method
    virtual GarlicBread *createGarlicBread(string &type) = 0; // product #2 ka factory method
    virtual ~MealFactory() {}                                 // base pointer se delete ke liye
};

// ============================================================================
//  SECTION 4: CONCRETE FACTORIES — har ek APNI FAMILY ki specialist
// ----------------------------------------------------------------------------
//  SinghBurger sirf NORMAL products banati hai, KingBurger sirf WHEAT.
//  Ek factory ke andar dusri family ka koi product ban hi nahi sakta —
//  yahi structural guarantee hai jo do alag factories me nahi milti.
// ============================================================================

// --- Concrete Factory #1: Singh — NORMAL theme family ---
class SinghBurger : public MealFactory
{
public:
    // Normal bun burgers hi banayega — wheat ka naam-o-nishan nahi
    Burger *createBurger(string &type) override
    {
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

    // Normal garlic bread hi banayega — burger ke saath MATCHING theme
    GarlicBread *createGarlicBread(string &type) override
    {
        if (type == "basic")
        {
            return new BasicGarlicBread();
        }
        else if (type == "cheese")
        {
            return new CheeseGarlicBread();
        }
        else
        {
            cout << "Invalid Garlic bread type! " << endl;
            return nullptr;
        }
    }
};

// --- Concrete Factory #2: King — WHEAT theme family ---
class KingBurger : public MealFactory
{
public:
    // Sirf wheat burgers — same type strings, alag family ke objects
    Burger *createBurger(string &type) override
    {
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

    // Sirf wheat garlic bread — burger se guaranteed matching!
    GarlicBread *createGarlicBread(string &type) override
    {
        if (type == "basic")
        {
            return new BasicWheatGarlicBread();
        }
        else if (type == "cheese")
        {
            return new CheeseWheatGarlicBread();
        }
        else
        {
            cout << "Invalid Garlic bread type! " << endl;
            return nullptr;
        }
    }
};

// ============================================================================
//  SECTION 5: CLIENT CODE — main()
// ----------------------------------------------------------------------------
//  Client ek baar THEME choose karta hai (kaunsi factory), phir us
//  factory se jo bhi products maange — sab same family ke milenge.
// ============================================================================
int main()
{
    string burgerType = "basic";
    string garlicBreadType = "cheese";

    // THEME DECISION — sirf yahi ek jagah hai jahan concrete factory ka
    // naam aata hai. KingBurger choose kiya = poora meal WHEAT theme ka.
    // new SinghBurger() kar do to poora meal normal ho jayega — bas
    // ek word ka change!
    MealFactory *mealFactory = new KingBurger();

    // Ab dono products SAME factory se aa rahe hain — isliye dono
    // guaranteed wheat family ke honge. Mix hona possible hi nahi.
    Burger *burger = mealFactory->createBurger(burgerType);
    GarlicBread *garlicBread = mealFactory->createGarlicBread(garlicBreadType);

    burger->prepare();      // → BasicWheatBurger::prepare()
    garlicBread->prepare(); // → CheeseWheatGarlicBread::prepare()

    // Cleanup — teeno interfaces ke virtual destructors ki wajah se safe.
    delete burger;
    delete garlicBread;
    delete mealFactory;

    return 0;
}

// ============================================================================
//  EXPECTED OUTPUT (KingBurger factory, burger="basic", bread="cheese"):
// ----------------------------------------------------------------------------
//  Preparing Basic Wheat Burger with bun, patty, and ketchup!
//  Preparing Cheese Wheat Garlic Bread with extra cheese and butter!
//
//  Dono WHEAT family ke — theme consistent! Yahi Abstract Factory ka point hai.
// ============================================================================
