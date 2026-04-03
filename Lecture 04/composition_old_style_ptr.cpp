#include <iostream>
using namespace std;

class A
{
public:
    void method1()
    {
        cout << "Method1 of Class A called" << endl;
    }

    // Destructor (optional here, but good practice)
    ~A()
    {
        cout << "A destroyed" << endl;
    }
};

class B
{
private:
    A *a; // raw pointer (old LLD style)

public:
    // Constructor
    B()
    {
        // manually memory allocate
        a = new A();

        // Explanation:
        // new A() → heap pe object create karta hai
        // 'a' uska pointer hold karta hai
    }

    void method2()
    {
        cout << "Method2 of Class B called" << endl;
    }

    A *getA()
    {
        return a; // access de rahe hain (ownership B ke paas hi hai)
    }

    // Destructor (VERY IMPORTANT ⚠️)
    ~B()
    {
        // manually memory free karna padega
        delete a;

        cout << "B destroyed and A memory freed" << endl;
    }
};

int main()
{
    // B object heap pe create
    B *b = new B();

    b->method2();
    b->getA()->method1();

    // manually delete karna padega
    delete b;

    return 0;
}