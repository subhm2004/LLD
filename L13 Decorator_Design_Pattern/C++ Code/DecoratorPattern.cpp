#include <bits/stdc++.h>

using namespace std;

// Component Interface: defines a common interface for Mario and all power-up decorators.
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

// Abstract Decorator: Character_Decorator "is-a" Charatcer and "has-a" Character.
class Character_Decorator : public Character
{
protected:
    Character *character; // Wrapped component
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

    delete mario;

    return 0;
}
