/**
 * AGGREGATION — weak "Has-a"; both INDEPENDENT lifetimes
 * Car has Engine* but does NOT own (delete) engine
 * Engine can outlive Car
 * UML: hollow diamond ◇ on Car side
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
    Engine* engine;  // pointer — aggregation (shared / external lifetime)

public:
    Car(string m, Engine* e) : model(m), engine(e) {
        cout << "[Car] created: " << model << " (uses external engine)\n";
    }

    ~Car() {
        cout << "[Car] destroyed: " << model << " (engine NOT deleted here)\n";
    }

    void drive() const {
        if (engine) engine->start();
        cout << "[Car] " << model << " driving\n";
    }
};

int main() {
    Engine v8("V8-Petrol");  // Engine exists OUTSIDE car

    {
        Car sedan("Honda City", &v8);
        sedan.drive();
    }  // Car destroyed — Engine still alive

    cout << "--- Car gone, engine still usable ---\n";
    v8.start();

    return 0;  // Engine destroyed at end of main
}
