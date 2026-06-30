/**
 * ============================================================================
 *  02_Aggregation.cpp  —  AGGREGATION (weak Has-A, shared/external lifetime)
 * ----------------------------------------------------------------------------
 *  Aggregation = "whole" ke paas "part" hai, par part ki lifetime BAHAR se
 *  control hoti hai. Whole destroy ho to part zinda reh sakta hai (kisi aur
 *  whole me reuse ho sakta hai). Yahan Car ke paas Engine* hai par Car usse
 *  delete NAHI karti — Engine Car ke bahar bana aur Car ke baad bhi zinda.
 *  UML: hollow diamond ◇ Car ki taraf.
 * ============================================================================
 */
#include <iostream>
#include <string>
using namespace std;

class Engine {
    string type;
public:
    Engine(string t) : type(t) {
        cout << "[Engine] created: " << type << "\n";
    }
    ~Engine() { cout << "[Engine] destroyed: " << type << "\n"; }
    void start() const { cout << "[Engine] " << type << " starting...\n"; }
};

class Car {
    string model;
    // Aggregation: pointer hold karte hain, par OWN nahi karte (external lifetime).
    Engine* engine;

public:
    // Engine bahar se inject hota hai (constructor injection) — Car banati nahi.
    Car(string m, Engine* e) : model(m), engine(e) {
        cout << "[Car] created: " << model << " (uses external engine)\n";
    }

    // Important: destructor me engine ko delete NAHI karte — yahi aggregation.
    ~Car() {
        cout << "[Car] destroyed: " << model << " (engine NOT deleted here)\n";
    }

    void drive() const {
        if (engine) engine->start();
        cout << "[Car] " << model << " driving\n";
    }
};

int main() {
    Engine v8("V8-Petrol");  // Engine Car ke BAHAR banaya

    {
        Car sedan("Honda City", &v8); // Engine inject kiya
        sedan.drive();
    }  // <-- Car yahan destroy ho gayi, par Engine abhi bhi zinda hai

    cout << "--- Car gone, engine still usable ---\n";
    v8.start(); // proof: engine ne car ko outlive kiya

    return 0;  // Engine ab main ke end pe destroy hoga
}
