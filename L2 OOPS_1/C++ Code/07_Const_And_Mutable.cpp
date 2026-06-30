// ============================================================================
//  07_Const_And_Mutable.cpp  —  const & mutable
// ----------------------------------------------------------------------------
//  `const` = immutability ka contract. Const method object ka state nahi badlta
//  (logical const) — compiler enforce karta hai. `mutable` = ek exception: aisi
//  field jo const method ke andar bhi badli ja sakti hai (jaise cache, counter,
//  ya mutex) — kyunki woh "logical" state ka hissa nahi hoti.
// ============================================================================
#include <iostream>
using namespace std;

class TemperatureSensor {
    double celsius;
    mutable int readCount;  // mutable — OK to change in const method

public:
    TemperatureSensor(double c) : celsius(c), readCount(0) {}

    double getCelsius() const {
        readCount++;
        return celsius;
    }

    void setCelsius(double c) {  // non-const — modifies state
        celsius = c;
    }
};

void printReading(const TemperatureSensor& s) {
    cout << "Reading: " << s.getCelsius() << " C\n";
}

int main() {
    const TemperatureSensor outdoor(36.5);
    printReading(outdoor);
    printReading(outdoor);
    // outdoor.setCelsius(40);  // ERROR: const object
    return 0;
}
