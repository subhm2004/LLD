#include <iostream>
#include <string>

using namespace std;

// ------------------------------------------------------------
// Bridge Pattern ka core idea:
// "Abstraction" (Car types) ko "Implementation" (Engine types) se
// alag hierarchy me rakho, taki dono independently grow kar sakein.
//
// Agar Bridge na ho, to SedanPetrol, SedanDiesel, SedanElectric,
// SUVPetrol, SUVDiesel, SUVElectric ... combinational class explosion ho jata.
// Bridge se hum composition use karte hain inheritance explosion ki jagah.
// ------------------------------------------------------------

// Implementation Hierarchy (Low-Level Details side)
// Engine abstraction batata hai ki "start" operation available hona chahiye.
class Engine {
public:
    // Pure virtual: har concrete engine apna start behavior define karega.
    virtual void start() = 0;

    // Virtual destructor important hai:
    // base pointer (Engine*) se delete karne par proper derived destructor chale.
    virtual ~Engine() {}
};

// Concrete Implementor 1: Petrol engine ka specific behavior.
class PetrolEngine : public Engine {
public:
    void start() override {
        cout << "Petrol engine starting with ignition!" << endl;
    }
};

// Concrete Implementor 2: Diesel engine ka specific behavior.
class DieselEngine : public Engine {
public:
    void start() override {
        cout << "Diesel engine roaring to life!" << endl;
    }
};

// Concrete Implementor 3: Electric engine ka specific behavior.
class ElectricEngine : public Engine {
public:
    void start() override {
        cout << "Electric engine powering up silently!" << endl;
    }
};

// Abstraction Hierarchy (High-Level side)
// Car ko engine implementation details nahi pata,
// bas Engine interface ke through kaam karta hai.
class Car {
protected:
    // Bridge reference/pointer:
    // Car abstraction -> Engine implementation ko connect kar raha hai.
    Engine* engine;
public:
    // Constructor injection:
    // runtime par decide hota hai ki kaunsa engine plug-in karna hai.
    Car(Engine* e) {
        engine = e;
    }

    // Har car apna drive style alag define karegi.
    virtual void drive() = 0;

    // Note: yaha virtual destructor add karna best practice hota hai
    // agar Car* se delete karna ho (is demo me deletes commented hain).
};

// Refined Abstraction 1: Sedan
// Sedan "what to do while driving" define karti hai,
// engine start ka real work implementation hierarchy karti hai.
class Sedan : public Car {
public:
    Sedan(Engine* e) : Car(e) {}

    void drive() override {
        // Delegation to implementor side (Bridge ki jaan yahi hai).
        engine->start();
        cout << "Driving a Sedan on the highway." << endl;
    }
};

// Refined Abstraction 2: SUV
class SUV : public Car {
public:
    SUV(Engine* e) : Car(e) {}

    void drive() override {
        // Same abstraction operation, but implementation swap ho sakta hai.
        engine->start();
        cout << "Driving an SUV off-road." << endl;
    }
};

int main() {
    // Step 1: Engine implementations banao (implementation side objects).
    Engine* petrolEng = new PetrolEngine();
    Engine* dieselEng = new DieselEngine();
    Engine* electricEng = new ElectricEngine();

    // Step 2: Car abstractions banao aur engine inject karo.
    // Dekho: same Car type me different engines attach ho sakte hain.
    Car* mySedan = new Sedan(petrolEng);
    Car* mySUV = new SUV(electricEng);
    Car* yourSUV = new SUV(dieselEng); // diesel engine attached SUV

    // Step 3: Use abstractions.
    // Runtime par bridge decide karta hai kaunsa engine behavior chalega.
    mySedan->drive();   // Sedan + PetrolEngine
    mySUV->drive();     // SUV + ElectricEngine
    yourSUV->drive();   // SUV + DieselEngine

    // Step 4: Clean up (important in raw pointer style).
    // Yaha intentionally commented hai demo simplicity ke liye,
    // but production/interview-quality code me delete ya smart pointers use karo.
    // delete mySedan;
    // delete mySUV;
    // delete yourSUV;
    // delete petrolEng;
    // delete dieselEng;
    // delete electricEng;

    return 0;
}
