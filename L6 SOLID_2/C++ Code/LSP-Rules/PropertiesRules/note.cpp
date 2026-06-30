// ============================================================================
//  note.cpp  —  `final` keyword note (LSP ke context me)
// ----------------------------------------------------------------------------
//  Agar tum chahte ho ki koi class inherit hi na ki ja sake (taaki koi subtype
//  LSP/invariants tod hi na paaye), to use `final` mark kar do. `final` class
//  ko koi inherit nahi kar sakta -> neeche Dog : Animal compile ERROR dega.
//  (Yeh file intentionally error dikhati hai — concept samajhne ke liye.)
// ============================================================================
#include <iostream>
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