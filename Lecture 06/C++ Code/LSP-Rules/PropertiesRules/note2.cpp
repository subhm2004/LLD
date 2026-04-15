#include <iostream> // yha pr bhi history constraint break ho rha hai
using namespace std;

class Animal
{
public:
    virtual void sound() final
    { // ❌ Override nahi kar sakte
        cout << "Animal sound" << endl;
    }
};

class Dog : public Animal
{
public:
    // ❌ ERROR: Cannot override final function
    void sound() override
    {
        cout << "Dog barks" << endl;
    }
};

int main()
{
    Animal *a = new Animal();
    a->sound();
    return 0;
}