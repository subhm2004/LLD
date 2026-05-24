/**
 * L2 — const: immutability contracts
 * const method = won't modify object state (logical const)
 * mutable = can change even in const method (caches, mutex)
 */
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
