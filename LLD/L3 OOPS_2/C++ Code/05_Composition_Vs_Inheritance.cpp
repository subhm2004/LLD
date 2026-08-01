// ============================================================================
//  05_Composition_Vs_Inheritance.cpp  —  Composition (HAS-A) vs Inheritance (IS-A)
// ----------------------------------------------------------------------------
//  Golden rule: "Favor composition over inheritance". Inheritance tabhi jab
//  sach me IS-A ho (Dog IS-A Animal). Agar relationship HAS-A hai (Car HAS-A
//  Engine), to composition use karo — Car IS Engine kabhi mat bolo. Composition
//  loose coupling + runtime swap deta hai; inheritance tight aur rigid hota hai.
// ============================================================================
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
