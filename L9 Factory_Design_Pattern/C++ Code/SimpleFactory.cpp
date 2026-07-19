// ============================================================================
//  SimpleFactory.cpp  —  Simple Factory (Factory family ka sabse basic roop)
// ----------------------------------------------------------------------------
//  BHAI SIMPLE FUNDA:
//  Client (main) ko khud `new BasicBurger()` nahi karna chahiye. Kyun?
//  Kyunki agar client har concrete class ko jaanta hai, to:
//    1. Naya burger type aaya → HAR client file me if-else edit karo
//    2. Client concrete classes se TIGHTLY COUPLED ho gaya
//    3. Creation logic (validation, defaults) har jagah duplicate
//
//  SOLUTION (Simple Factory):
//  Object banane ka saara kaam EK jagah — factory class — me daal do.
//  Client bas type string deta hai ("basic"/"standard"/"premium") aur
//  factory sahi object bana ke Burger* (base pointer) return karti hai.
//  Client ko concrete class ka NAAM tak pata nahi hota!
//
//  DHYAN RAKHO:
//  - Simple Factory GoF book ka official pattern NAHI hai — ye ek common
//    idiom hai. Par interview me isse hi shuruaat hoti hai.
//  - Iska weakness: naya type aaye to factory ke if-else me edit karna
//    padta hai (OCP weak). Isko FactoryMethod.cpp aur AbstractFactory.cpp
//    improve karte hain — teeno files ek evolution story hai.
//
//  Analogy: Fast food counter — tum bas "standard" bolte ho, kitchen
//  (factory) andar decide karti hai kaunsa burger kaise banana hai.
// ============================================================================
#include <bits/stdc++.h>

using namespace std;

// ============================================================================
//  SECTION 1: PRODUCT INTERFACE
// ----------------------------------------------------------------------------
//  Saare burgers ka common contract. Factory isi base type ka pointer
//  return karegi — isi wajah se client ko concrete class nahi jaanni padti.
// ============================================================================
class Burger
{
public:
    // Pure virtual (= 0) — Burger abstract class hai, iska object nahi
    // ban sakta. Har concrete burger ko prepare() dena HI padega.
    virtual void prepare() = 0;

    // Virtual destructor ZAROORI — client `Burger*` (base pointer) se
    // delete karega. Virtual na ho to derived class ka destructor call
    // nahi hota → undefined behavior / memory leak.
    virtual ~Burger() {}
};

// ============================================================================
//  SECTION 2: CONCRETE PRODUCTS
// ----------------------------------------------------------------------------
//  Ye actual products hain jo factory banayegi. Har ek apna prepare()
//  implement karta hai — polymorphism ke through sahi wala call hota hai.
// ============================================================================

// --- Product #1: Sabse sasta burger ---
class BasicBurger : public Burger
{
public:
    void prepare() override
    {
        cout << "Preparing Basic Burger with bun, patty, and ketchup!" << endl;
    }
};

// --- Product #2: Beech wala option ---
class StandardBurger : public Burger
{
public:
    void prepare() override
    {
        cout << "Preparing Standard Burger with bun, patty, cheese, and lettuce!" << endl;
    }
};

// --- Product #3: Sabse mehenga, secret sauce wala ---
class PremiumBurger : public Burger
{
public:
    void prepare() override
    {
        cout << "Preparing Premium Burger with gourmet bun, premium patty, cheese, lettuce, and secret sauce!" << endl;
    }
};

// ============================================================================
//  SECTION 3: SIMPLE FACTORY
// ----------------------------------------------------------------------------
//  Pattern ka DIL — saari `new` calls ab sirf is class ke andar hain.
//  Client ke liye creation ka decision ye class leti hai.
//
//  NOTE: Ye if-else ladder hi Simple Factory ka WEAK POINT hai —
//  naya burger type (jaise "veggie") aaya to YE method edit karna
//  padega. Matlab Open/Closed Principle break hota hai (closed for
//  modification nahi raha). FactoryMethod.cpp me dekhoge ki is problem
//  ko subclassing se kaise solve karte hain.
// ============================================================================
class BurgerFactory
{
public:
    // Type string lo → sahi concrete burger banao → BASE pointer return karo.
    // Return type Burger* hai (concrete nahi) — yahi abstraction hai jo
    // client ko concrete classes se decouple karti hai.
    Burger *createBurger(string &type)
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
            // Galat type pe nullptr — production me exception ya
            // optional<> better hota. Client ko null-check karna chahiye.
            cout << "Invalid burger type! " << endl;
            return nullptr;
        }
    }
};

// ============================================================================
//  SECTION 4: CLIENT CODE — main()
// ----------------------------------------------------------------------------
//  Dekho client kitna SAAF hai — na koi if-else, na kisi concrete burger
//  class ka naam. Bas factory se maango aur use karo.
// ============================================================================
int main()
{
    // Ye string user input / config / API se aa sakti hai — runtime decision!
    string type = "standard";

    // Factory banao (Simple Factory me ye concrete class hi hai —
    // yahi ek aur difference hai Factory Method se, jahan ye abstract hota hai)
    BurgerFactory *myBurgerFactory = new BurgerFactory();

    // MAGIC LINE: client ko StandardBurger ka naam tak nahi pata —
    // bas "standard" bola aur Burger* mil gaya. Creation ka saara
    // decision factory ke andar hua.
    Burger *burger = myBurgerFactory->createBurger(type);

    // Polymorphism: Burger* pe prepare() call karo → virtual dispatch
    // se StandardBurger::prepare() chalega.
    burger->prepare();

    // Cleanup — Burger ka virtual destructor hone se sahi derived
    // destructor chalega. (Production me unique_ptr use karo —
    // manual delete bhoolne ka risk hi khatam.)
    delete burger;
    delete myBurgerFactory;

    return 0;
}

// ============================================================================
//  EXPECTED OUTPUT (type = "standard"):
// ----------------------------------------------------------------------------
//  Preparing Standard Burger with bun, patty, cheese, and lettuce!
// ============================================================================
