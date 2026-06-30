// ============================================================================
//  SimpleFactory.cpp  —  Simple Factory (Factory pattern ka basic roop)
// ----------------------------------------------------------------------------
//  Simple Factory = object banane ka kaam ek hi jagah (factory class) me
//  centralize karna. Client `new BasicBurger()` khud nahi karta — bas type
//  string deta hai aur factory sahi object bana ke deta hai. Faayda: creation
//  logic ek jagah. Nuksaan (OCP weak): naya burger type aaya to factory ke
//  if-else me edit karna padega. (Aage FactoryMethod/AbstractFactory isse
//  improve karte hain.)
// ============================================================================
#include <bits/stdc++.h>

using namespace std;

// Product interface — saare burgers prepare() denge.
class Burger
{
public:
    virtual void prepare() = 0; // pure virtual
    virtual ~Burger() {}        // virtual destructor (base pointer se delete safe)
};

// Concrete products
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

// Simple Factory: type ke hisaab se sahi concrete burger bana ke deta hai.
// (Yeh if-else hi OCP-weak hissa hai — naya type = yahan edit.)
class BurgerFactory
{
public:
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
            cout << "Invalid burger type! " << endl;
            return nullptr;
        }
    }
};

int main()
{
    string type = "standard";

    BurgerFactory *myBurgerFactory = new BurgerFactory();

    // Client ko concrete class ka naam nahi pata — bas factory se maangta hai.
    Burger *burger = myBurgerFactory->createBurger(type);

    burger->prepare();
    delete burger;
    delete myBurgerFactory;

    return 0;
}
