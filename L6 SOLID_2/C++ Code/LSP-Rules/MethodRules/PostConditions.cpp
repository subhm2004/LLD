// ============================================================================
//  PostConditions.cpp  —  LSP Method Rule: Post-conditions
// ----------------------------------------------------------------------------
//  Post-condition = method chalne ke BAAD jo sach hona chahiye (output guarantee).
//  LSP rule: subtype post-condition ko STRENGTHEN kar sakta hai, WEAKEN nahi.
//  Matlab override caller ko utna ya usse ZYADA guarantee de — kam nahi (jaise
//  base "balance >= 0 rakhega" promise karta tha to derived usse kam promise na
//  kare). Yaad rakho: "promise no less" (post-conditions sirf tight ho sakti hain).
// ============================================================================
#include <bits/stdc++.h>

using namespace std;

// A Postcondition must be statisfied after a method is executed.
// Sub classes can strengthen the Postcondition but cannot weaken it.

class Car
{
protected:
    int speed;

public:
    Car()
    {
        speed = 0;
    }

    void accelerate()
    {
        cout << "Accelerating" << endl;
        speed += 20;
    }

    // PostCondition : Speed must reduce after brake
    virtual void brake()
    {
        cout << "Applying brakes" << endl;
        speed -= 20;
    }
};

// Subclass can strengthen postcondition - Does not violate LSP
class HybridCar : public Car
{
private:
    int charge;

public:
    HybridCar() : Car()
    {
        charge = 0;
    }

    // PostCondition : Speed must reduce after brake
    // PostCondition : Charge must increase.
    void brake()
    {
        cout << "Applying brakes" << endl;
        speed -= 20;
        charge += 10;
    }
};

int main()
{
    Car *hybridCar = new HybridCar();
    hybridCar->brake(); // Works fine: HybridCar reduces speed and also increases charge.

    // Client feels no difference in substituting Hybrid car in place of Car.

    return 0;
}
