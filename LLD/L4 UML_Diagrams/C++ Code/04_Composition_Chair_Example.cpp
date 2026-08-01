// ============================================================================
//  04_Composition_Chair_Example.cpp  —  Composition ka real example (Chair)
// ----------------------------------------------------------------------------
//  Chair "HAS-A" Seat, Arms, Wheels, Cover — ek whole jo kai parts se bana hai.
//  Saare parts Chair ke constructor me bante hain aur Chair destroy hote hi
//  (unique_ptr ki wajah se) automatic destroy ho jaate hain -> strong
//  composition. UML: filled diamond ◆ Chair ki taraf har part ke saath.
// ============================================================================
#include <iostream>
#include <memory>

using namespace std;

// Individual components (Chair ke parts) — har ek apni info print karta hai.

class Seat
{
public:
    void info()
    {
        cout << "Seat is comfortable\n";
    }
};

class Arms
{
public:
    void info()
    {
        cout << "Arms for support\n";
    }
};

class Wheels
{
public:
    void info()
    {
        cout << "Wheels for movement\n";
    }
};

class Cover
{
public:
    void info()
    {
        cout << "Cover for protection\n";
    }
};

// Chair HAS-A parts (Composition) — unique_ptr se exclusive ownership.
class Chair
{
private:
    unique_ptr<Seat> seat;
    unique_ptr<Arms> arms;
    unique_ptr<Wheels> wheels;
    unique_ptr<Cover> cover;

public:
    // Constructor -> saare parts Chair ke saath hi create hote hain.
    Chair()
    {
        seat = make_unique<Seat>();
        arms = make_unique<Arms>();
        wheels = make_unique<Wheels>();
        cover = make_unique<Cover>();
    }
    void info()
    {
        cout << "This is a chair is made by Shubham Malik :" << endl;
    }

    // Chair apne parts ko delegate karke details dikhati hai.
    void showChairDetails()
    {
        seat->info();
        arms->info();
        wheels->info();
        cover->info();
    }

    // Aise getter de sakte hain, par isse encapsulation kamzor hoti hai
    // (andar ka part bahar expose ho jaata hai) — generally avoid karo.
    Wheels *getWheels()
    {
        return wheels.get();
    }
    // Chair destroy -> seat/arms/wheels/cover sab automatic destroy (unique_ptr).
};

int main()
{
    auto chair = make_unique<Chair>();

    chair->info();
    chair->showChairDetails();

    return 0; // chair scope-end pe destroy -> saare parts bhi free
}
