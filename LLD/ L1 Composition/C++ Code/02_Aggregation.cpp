/**
 * ============================================================================
 *  02_Aggregation.cpp  —  AGGREGATION (object relationship #2, weak Has-A)
 * ----------------------------------------------------------------------------
 *  Aggregation = "WHOLE ke paas PART hai, par part ki lifetime BAHAR se control
 *                 hoti hai. Whole marne pe part zinda reh sakta (reuse ho sakta)."
 *
 *  Asli duniya ka example: Car aur Engine 🚗
 *    Car ke paas engine hai (uses it), par engine Car ke BAHAR banaya jaata
 *    hai aur Car ke scrap hone ke baad bhi engine nikaal ke kisi aur car me
 *    laga sakte ho. Engine ki zindagi Car pe depend NAHI karti — SHARED/external.
 *
 *  ┌──────────────────────────────────────────────────────────────────────────┐
 *  │  ASSOCIATION vs AGGREGATION (dono me pointer field hota — confusion!):  │
 *  │                                                                          │
 *  │  Association -> "peer" relationship (Teacher-Student — barabari ka)     │
 *  │  Aggregation -> "whole-part" relationship (Car HAS Engine — part-of)    │
 *  │                                                                          │
 *  │  Aggregation ek SPECIALIZED association hai jisme "whole-part" ka        │
 *  │  matlab hota. Dono me part ka ownership whole ke paas NAHI hota —        │
 *  │  yahi dono ko composition se alag karta hai.                            │
 *  │                                                                          │
 *  │  UML: Aggregation = HOLLOW diamond ◇ (khaali, weak ownership)           │
 *  │       Composition = FILLED diamond ◆ (bhara, strong ownership)          │
 *  └──────────────────────────────────────────────────────────────────────────┘
 *
 *  📌 AGGREGATION ki pehchaan (is file me):
 *  - Car ke paas Engine* field hai (has-a)
 *  - Engine CONSTRUCTOR INJECTION se aata (Car ke bahar banaya, inject kiya)
 *  - Car ka destructor Engine ko `delete` NAHI karta (external lifetime)
 *  - Engine Car ko OUTLIVE karta hai (Car marne ke baad bhi zinda)
 *  Isi demo me niche dekhoge: Car scope khatam, par Engine abhi bhi usable!
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
    Engine v8("V8-Petrol");  // Engine Car ke BAHAR banaya (external lifetime)

    {
        Car sedan("Honda City", &v8); // Engine INJECT kiya (constructor injection)
        sedan.drive();
    }  // <-- inner scope khatam -> Car DESTROY, par Engine abhi zinda!
       //     (Car ke destructor me engine delete NAHI hota — aggregation)

    // >>> AGGREGATION KA PROOF <<<
    // Car mar gayi par Engine abhi bhi kaam kar raha — Engine ne Car ko
    // OUTLIVE kiya. Ab isi engine ko dusri car me bhi laga sakte the (reuse).
    cout << "--- Car gone, engine still usable ---\n";
    v8.start(); // proof: engine zinda hai

    return 0;  // Engine ab YAHAN (main ke end pe) destroy hoga
}

/**
 * EXPECTED OUTPUT (dhyan do Engine ka destructor SABSE LAST me chalega):
 *   [Engine] created: V8-Petrol
 *   [Car] created: Honda City (uses external engine)
 *   [Engine] V8-Petrol starting...
 *   [Car] Honda City driving
 *   [Car] destroyed: Honda City (engine NOT deleted here)   <- Car gayi
 *   --- Car gone, engine still usable ---
 *   [Engine] V8-Petrol starting...                          <- Engine zinda!
 *   [Engine] destroyed: V8-Petrol                           <- main ke end pe
 *
 *   ^ "[Car] destroyed" ke baad "[Engine] destroyed" aana = aggregation proof.
 *   Composition hota to Car ke saath hi Engine bhi mar jaata.
 */
