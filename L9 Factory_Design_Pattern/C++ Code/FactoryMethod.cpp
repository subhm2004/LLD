// ============================================================================
//  FactoryMethod.cpp  —  Factory Method Pattern (Creational)
// ----------------------------------------------------------------------------
//  Factory Method = object banane ka faisla SUBCLASS pe chhod do. Ek abstract
//  BurgerFactory hai jo createBurger() declare karta hai; har concrete factory
//  (SinghBurger = normal burgers, KingBurger = wheat burgers) apne hisaab se
//  banata hai. Naya factory/variant add karna ho to NAYI factory class banao —
//  existing code edit nahi (Simple Factory se behtar OCP). Client sirf abstract
//  BurgerFactory* ke saath kaam karta hai.
// ============================================================================
#include <bits/stdc++.h>

using namespace std;

// Product Class and subclasses
class Burger
{
public:
    virtual void prepare() = 0; // Pure virtual function
    virtual ~Burger() {}        // Virtual destructor
};

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

// Factory and its concretions
class BurgerFactory
{
public:
    virtual Burger *createBurger(string &type) = 0;
    virtual ~BurgerFactory() {} // Virtual destructor
};

class SinghBurger : public BurgerFactory
{
public:
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
};

class KingBurger : public BurgerFactory
{
public:
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
};
// client
int main()
{
    string type = "basic";

    BurgerFactory *myFactory = new SinghBurger();

    Burger *burger = myFactory->createBurger(type);

    burger->prepare();
    delete burger;
    delete myFactory;

    return 0;
}
