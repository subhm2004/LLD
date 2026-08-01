/*
=====================================================================
OVERUSE OF INHERITANCE ANTI-PATTERN + BONUS: NULL OBJECT PATTERN SOLUTION
=====================================================================
THEORY (Inheritance Overuse):
Inheritance ka use tabhi karna chahiye jab genuinely "IS-A"
relationship ho (jaise Duck IS-A Bird). Agar relationship "HAS-A"
(Composition) type ki hai, lekin humne zabardasti Inheritance laga
di, to isse design galat ho jata hai.

Iska classic violation hai "Liskov Substitution Principle" (SOLID
ka "L") - jo kehta hai ki child class, parent class ki jagah
seamlessly use ho sakni chahiye, bina kisi unexpected behavior ke.

PROBLEM:
- Child class parent ke kuch methods ko meaningfully implement
  nahi kar pati (jaise ToyDuck ke paas fly() nahi hona chahiye)
- Exception throw karna padta hai jo bad design ka signal hai
- Code fragile ho jata hai

SOLUTION APPROACH:
- Composition over Inheritance principle follow karo
- Interfaces ko chhote-chhote, specific banao (Interface
  Segregation Principle) jaise separate Flyable interface banao
  jo sirf flying birds implement karein

THEORY (Null Object Pattern):
C++ mein null pointers bahut dangerous hote hain. Agar inko
properly handle na kiya jaye toh crash aa jata hai. Null Object
Pattern ek "do-nothing" object provide karta hai taaki baar-baar
null check na karna pade.
=====================================================================
*/

#include <iostream>
#include <stdexcept>
#include <memory>
using namespace std;

// ---------------------------------------------------------------
// ANTI-PATTERN VERSION: zabardasti Inheritance, ToyDuck fly nahi
// kar sakta lekin usse Bird se inherit karaya gaya hai
// ---------------------------------------------------------------
class Bird
{
public:
    virtual void fly()
    {
        cout << "Flying in the sky" << endl;
    }
    virtual ~Bird() = default;
};

class Duck : public Bird
{
public:
    void quack()
    {
        cout << "Quack quack!" << endl;
    }
};

class ToyDuck : public Duck
{
public:
    void fly() override
    {
        // ToyDuck asal me fly nahi kar sakta, isliye exception
        // throw karna pad raha hai - ye bataata hai design galat hai
        throw logic_error("Toy ducks don't fly!");
    }
};

// ---------------------------------------------------------------
// FIXED VERSION: Composition + Interface Segregation
// Flyable interface alag kar diya, sirf wahi birds implement
// karenge jo genuinely fly kar sakte hain
// ---------------------------------------------------------------
class Flyable
{
public:
    virtual void fly() = 0;
    virtual ~Flyable() = default;
};

class BirdFixed
{
public:
    virtual void makeSound()
    {
        cout << "Some generic bird sound" << endl;
    }
    virtual ~BirdFixed() = default;
};

class DuckFixed : public BirdFixed, public Flyable
{
public:
    void makeSound() override
    {
        cout << "Quack quack!" << endl;
    }
    void fly() override
    {
        cout << "Duck is flying" << endl;
    }
};

class ToyDuckFixed : public BirdFixed
{
    // ToyDuckFixed Flyable implement hi nahi karta, kyunki wo
    // genuinely fly nahi kar sakta - ab koi exception throw karne
    // ki zaroorat nahi, design hi sahi hai
public:
    void makeSound() override
    {
        cout << "Squeak squeak! (toy sound)" << endl;
    }
};

/*
=====================================================================
BONUS: NULL OBJECT PATTERN DEMO
=====================================================================
*/

// Base interface - sabhi commands isko implement karenge
class Command
{
public:
    virtual void execute() = 0;
    virtual ~Command() = default;
};

// Real command jo actually kuch kaam karta hai
class LightOnCommand : public Command
{
public:
    void execute() override
    {
        cout << "Light turned ON" << endl;
    }
};

// Null Object - ye interface implement karta hai lekin andar se
// khaali hai, "do nothing" behavior deta hai
class NoCommand : public Command
{
public:
    void execute() override
    {
        // Kuch nahi karta - safe default behavior
        cout << "No command assigned, doing nothing" << endl;
    }
};

// Client code jo Command interface use karta hai
class RemoteControl
{
private:
    unique_ptr<Command> cmd;

public:
    // Default initialization mein NoCommand assign kar do
    // (nullptr ki jagah) - isse object hamesha valid rahega
    RemoteControl()
    {
        cmd = make_unique<NoCommand>();
    }

    void setCommand(unique_ptr<Command> newCmd)
    {
        cmd = move(newCmd);
    }

    void pressButton()
    {
        // Yahan if (cmd != nullptr) check karne ki zarurat hi
        // nahi hai! cmd hamesha kuch na kuch valid object point
        // kar raha hoga
        cmd->execute();
    }
};

int main()
{
    cout << "--- Inheritance Overuse (Anti-Pattern) Demo ---" << endl;
    try
    {
        ToyDuck toy;
        toy.fly(); // ye exception throw karega
    }
    catch (exception &e)
    {
        cout << "Error: " << e.what() << endl;
    }

    cout << "\n--- Fixed Version (Composition + Interface Segregation) Demo ---" << endl;
    DuckFixed realDuck;
    realDuck.makeSound();
    realDuck.fly();

    ToyDuckFixed toyDuck;
    toyDuck.makeSound();
    // toyDuck.fly(); -> ye compile hi nahi hoga kyunki ToyDuckFixed
    // Flyable implement nahi karta - design-time pe hi galti pakdi jayegi

    cout << "\n--- Null Object Pattern Demo ---" << endl;
    RemoteControl remote;
    remote.pressButton(); // NoCommand chalega, crash nahi hoga

    remote.setCommand(make_unique<LightOnCommand>());
    remote.pressButton(); // Ab real command chalega

    return 0;
}