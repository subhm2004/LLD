/**
 * L2 — static data member + static member function
 * Belongs to CLASS, not each object. One shared copy.
 */
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
