// ============================================================================
//  DecoratorPattern.cpp  —  Decorator Design Pattern (Structural)
// ----------------------------------------------------------------------------
//  Decorator = ek object par naye features RUNTIME par "wrap" karke add karna,
//  bina uski class badle. Har decorator base se IS-A bhi hai aur usko HAS-A
//  bhi (wrap karta hai). Mario par HeightUp, GunPowerUp, StarPowerUp ek ke upar
//  ek lag jaate hain. Inheritance hota to 2^n subclasses banti (combinatorial
//  explosion) — decorator usse bachata hai, abilities stack ho jaati hain.
// ============================================================================
#include <bits/stdc++.h>

using namespace std;

// Component Interface: Mario aur saare power-up decorators ka common interface.
class Character
{
public:
    virtual string getAbilities() const = 0;
    virtual ~Character() {} // Virtual destructor
};

// Concrete Component: Basic Mario character with no power-ups.
class Mario : public Character
{
public:
    string getAbilities() const override
    {
        return "Mario";
    }
};

// Abstract Decorator: Character_Decorator "is-a realtionship" Charatcer and "has-a" Character.
class Character_Decorator : public Character
{
protected:
    Character *character; // Wrapped component (has-a relationship)
public:
    Character_Decorator(Character *c)
    {
        this->character = c;
    }
};

// Concrete Decorator: Height-Increasing Power-Up.
class HeightUp : public Character_Decorator
{
public:
    HeightUp(Character *c) : Character_Decorator(c) {}

    string getAbilities() const override
    {
        return character->getAbilities() + " with HeightUp";
    }
};

// Concrete Decorator: Gun Shooting Power-Up.
class GunPowerUp : public Character_Decorator
{
public:
    GunPowerUp(Character *c) : Character_Decorator(c) {}

    string getAbilities() const override
    {
        return character->getAbilities() + " with Gun";
    }
};

// Concrete Decorator: Star Power-Up (temporary ability).
class StarPowerUp : public Character_Decorator
{
public:
    StarPowerUp(Character *c) : Character_Decorator(c) {}

    string getAbilities() const override
    {
        return character->getAbilities() + " with Star Power (Limited Time)";
    }

    ~StarPowerUp()
    {
        cout << "Destroying StarPowerUp Decorator" << endl;
    }
};

int main()
{
    // Create a basic Mario character.
    Character *mario = new Mario();
    cout << "Basic Character: " << mario->getAbilities() << endl;

    // Decorate Mario with a HeightUp power-up.
    mario = new HeightUp(mario);
    cout << "After HeightUp: " << mario->getAbilities() << endl;

    // Decorate Mario further with a GunPowerUp.
    mario = new GunPowerUp(mario);
    cout << "After GunPowerUp: " << mario->getAbilities() << endl;

    // Finally, add a StarPowerUp decoration.
    mario = new StarPowerUp(mario);
    cout << "After StarPowerUp: " << mario->getAbilities() << endl;

    cout << endl;

    // just for learning see pg image in folder
    mario = new StarPowerUp(new GunPowerUp(new HeightUp(mario)));
    cout << "Just for learning :" << mario->getAbilities() << endl;

    delete mario;

    return 0;
}
