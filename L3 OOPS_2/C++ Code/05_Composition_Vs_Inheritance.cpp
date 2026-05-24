/**
 * L3 — Composition (HAS-A) vs Inheritance (IS-A)
 * Car HAS Engine — don't say Car IS Engine
 */
#include <iostream>
#include <string>
using namespace std;

class Engine {
    string type;
public:
    Engine(string t) : type(t) {}
    void start() const { cout << type << " engine started.\n"; }
};

// Composition — Car contains Engine
class Car {
    Engine engine;  // HAS-A
    string model;
public:
    Car(string m, string engineType) : model(m), engine(engineType) {}
    void drive() const {
        cout << model << ": ";
        engine.start();
    }
};

int main() {
    Car sedan("Honda City", "Petrol");
    sedan.drive();
    return 0;
}
