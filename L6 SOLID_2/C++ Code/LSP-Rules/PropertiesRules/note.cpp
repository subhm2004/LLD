#include <iostream> // yha pr history constraint follow nhi ho rha hai
using namespace std;

class Animal final // ❌ Is class ko inherit nahi kar sakte
{
public:
    void sound()
    {
        cout << "Animal sound" << endl;
    }
};

// ❌ ERROR: Cannot inherit from final class
class Dog : public Animal
{
};

int main()
{
    Animal a;
    a.sound();
    return 0;
}