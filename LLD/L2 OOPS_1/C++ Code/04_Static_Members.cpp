// ============================================================================
//  04_Static_Members.cpp  —  Static data members & static member functions
// ----------------------------------------------------------------------------
//  `static` member CLASS ka hota hai, har object ka nahi — saare objects ek hi
//  shared copy use karte hain (jaise total count). Static member FUNCTION bina
//  object ke (ClassName::func()) call ho sakta hai, par sirf static members hi
//  access kar sakta hai (uske paas `this` nahi hota).
// ============================================================================
#include <iostream>
using namespace std;

class CarFactory {
    static int carsProduced;   // declaration
    string brand;

public:
    CarFactory(string b) : brand(b) {
        carsProduced++;
    }

    static int getTotalCars() {  // no this — call via Class::method
        return carsProduced;
    }

    void show() const {
        cout << brand << " registered. Total cars: " << carsProduced << "\n";
    }
};

// Definition outside class (required for static data)
int CarFactory::carsProduced = 0;

int main() {
    CarFactory a("Tesla");
    CarFactory b("BMW");
    cout << "Via static method: " << CarFactory::getTotalCars() << "\n";
    a.show();
    b.show();
    return 0;
}
