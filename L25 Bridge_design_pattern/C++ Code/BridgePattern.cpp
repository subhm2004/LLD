// ============================================================================
//  BridgePattern.cpp  —  BRIDGE DESIGN PATTERN (Structural)
// ----------------------------------------------------------------------------
//  Bridge = "do alag-alag badhne wali hierarchies (Abstraction aur
//            Implementation) ko EK inheritance tree me thoosne ki jagah
//            DO ALAG trees banao, aur beech me ek POINTER ka pul (bridge)
//            daal do — dono independently grow kar sakein."
//
//  Asli duniya ka example: Car aur Engine 🚗
//    - Car TYPES badhte rehte hain:   Sedan, SUV, (kal Hatchback...)
//    - Engine TYPES bhi badhte hain:  Petrol, Diesel, Electric, (kal Hybrid...)
//    - Dono INDEPENDENT dimensions hain — koi bhi car, koi bhi engine!
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  BRIDGE KYUN? — bina iske class explosion 💥:                           │
//  │                                                                          │
//  │    Ek hi inheritance tree me sab thoonso:                               │
//  │      SedanPetrol, SedanDiesel, SedanElectric,                           │
//  │      SUVPetrol,   SUVDiesel,   SUVElectric      = 2 × 3 = 6 classes!    │
//  │                                                                          │
//  │    Ab Hatchback aayi -> +3 classes. Hybrid engine aaya -> +3 aur.       │
//  │    n car types × m engines = n×m classes — multiplication ka jail! 😵   │
//  │                                                                          │
//  │  BRIDGE se: n + m classes bas!                                          │
//  │    Car tree:    Car -> Sedan, SUV            (2 classes)                │
//  │    Engine tree: Engine -> Petrol/Diesel/Electric (3 classes)            │
//  │    Connection:  Car ke paas Engine* POINTER (yahi "bridge" hai!)        │
//  │    2 + 3 = 5 classes se saare 6 combos milte hain — runtime pe jodo!    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  PATTERN KE ROLES (GoF naming) is file me:
//    1. Abstraction            -> Car     : high-level interface (drive karo);
//                                           Engine* pointer HOLD karta hai
//    2. Refined Abstraction    -> Sedan, SUV : abstraction ke variants
//    3. Implementor (interface)-> Engine  : low-level operations ka contract (start)
//    4. Concrete Implementor   -> PetrolEngine, DieselEngine, ElectricEngine
//
//  ============================================================================
//   COMBINATION MATRIX — n + m classes se n × m combos
//   (har cell ek VALID runtime pairing hai — koi nayi class nahi banani padi!)
//  ----------------------------------------------------------------------------
//              | PetrolEngine    | DieselEngine    | ElectricEngine
//   -----------+-----------------+-----------------+------------------
//   Sedan      | new Sedan(pet)✅| new Sedan(dsl)  | new Sedan(elec)
//   SUV        | new SUV(pet)    | new SUV(dsl) ✅ | new SUV(elec) ✅
//   Hatchback  | (nayi Car class banao — ENGINES ki taraf KOI change nahi!)
//   (NAYA)     |
//
//   ✅ = is demo ke main() me use hua
//
//   📌 SABSE BADA RULE — DONO SIDES EK-DUSRE SE AZAAD HAIN:
//   Naya car type? -> Car tree me ek class. Engines untouched.
//   Naya engine?   -> Engine tree me ek class. Cars untouched.
//   Yahi "decouple abstraction from implementation so both can vary
//   independently" hai — GoF ki exact definition!
//
//   ⭐ BRIDGE vs STRATEGY (interview ka confusion #1):
//   Structure lagbhag SAME (composition + interface)! Farq INTENT ka hai:
//     Strategy = EK behavior/algorithm ko runtime pe SWAP karna
//                (kaam ka "tareeka" badalna — L8 wale robots)
//     Bridge   = DO POORI HIERARCHIES ko architecture level pe ALAG rakhna
//                (dono taraf classes BADHTI hain — yahi difference hai)
// ============================================================================
#include <iostream>
#include <string>

using namespace std;

// ============================================================================
//  SECTION 1: IMPLEMENTATION HIERARCHY — Engine (low-level side)
// ----------------------------------------------------------------------------
//  Ye "kaise kaam hota hai" wali side hai. Car ko iske andar ki details
//  se koi matlab nahi — bas start() ka contract chahiye.
// ============================================================================

// --- Implementor interface: har engine ka common contract ---
class Engine {
public:
    // Pure virtual — har concrete engine apna start behavior dega
    virtual void start() = 0;

    // Virtual destructor ZAROORI: Engine* (base pointer) se derived
    // engine delete hoga — virtual na ho to derived ka destructor
    // skip -> leak/UB. (Har polymorphic base ka rule!)
    virtual ~Engine() {}
};

// --- Concrete Implementor #1: Petrol ---
class PetrolEngine : public Engine {
public:
    void start() override {
        cout << "Petrol engine starting with ignition!" << endl;
    }
};

// --- Concrete Implementor #2: Diesel ---
class DieselEngine : public Engine {
public:
    void start() override {
        cout << "Diesel engine roaring to life!" << endl;
    }
};

// --- Concrete Implementor #3: Electric ---
// Naya engine add karna = BAS ye jitni classes. Car side ko pata bhi
// nahi chalta ki naya engine aa gaya!
class ElectricEngine : public Engine {
public:
    void start() override {
        cout << "Electric engine powering up silently!" << endl;
    }
};

// ============================================================================
//  SECTION 2: ABSTRACTION HIERARCHY — Car (high-level side)
// ----------------------------------------------------------------------------
//  Ye "kya karna hai" wali side hai (drive!). Engine ka kaam Engine
//  pointer ke through DELEGATE hota hai.
// ============================================================================

// --- Abstraction: Car ---
class Car {
protected:
    // >>> YAHI "BRIDGE" HAI! <<<
    // Ye ek pointer do poori hierarchies ko jodta hai:
    //   Car tree (Sedan/SUV)  <---bridge---> Engine tree (Petrol/Diesel/Electric)
    // Inheritance hota to compile-time pe jud jaata (SedanPetrol);
    // pointer hai to RUNTIME pe koi bhi combo ban sakta hai!
    Engine* engine;
public:
    // Constructor injection — kaunsa engine lagana hai, ye CLIENT
    // decide karta hai object banate waqt (composition ka power)
    Car(Engine* e) {
        engine = e;
    }

    // Har car type apna driving style dega
    virtual void drive() = 0;

    // NOTE: yahan virtual destructor hona chahiye (Car* se delete ke
    // liye) — is demo me deletes commented hain isliye chal gaya,
    // par production code me `virtual ~Car() {}` zaroor likho!
};

// --- Refined Abstraction #1: Sedan ---
class Sedan : public Car {
public:
    Sedan(Engine* e) : Car(e) {}

    void drive() override {
        // DELEGATION — Bridge ki jaan yahi line hai:
        // "engine start karna MERA kaam nahi, engine ka kaam hai."
        // Sedan ko pata bhi nahi kaunsa engine laga hai — virtual
        // dispatch sahi start() chala dega.
        engine->start();
        cout << "Driving a Sedan on the highway." << endl;
    }
};

// --- Refined Abstraction #2: SUV ---
// Naya car type add karna = BAS itni si class. Engine side untouched!
class SUV : public Car {
public:
    SUV(Engine* e) : Car(e) {}

    void drive() override {
        // Same delegation — par is baar jo bhi engine inject hua
        // (main me Electric/Diesel) wahi start hoga
        engine->start();
        cout << "Driving an SUV off-road." << endl;
    }
};

// ============================================================================
//  SECTION 3: CLIENT CODE — main()
// ----------------------------------------------------------------------------
//  Client dono sides ke objects banata hai aur RUNTIME pe jodta hai —
//  koi SedanPetrol/SUVElectric jaisi combo class kahin exist nahi karti!
// ============================================================================
int main() {
    // STEP 1: Implementation side ke objects (engines)
    Engine* petrolEng = new PetrolEngine();
    Engine* dieselEng = new DieselEngine();
    Engine* electricEng = new ElectricEngine();

    // STEP 2: Abstraction side ke objects — engine INJECT karke.
    // Dekho: SAME SUV class, ALAG engines — bina koi nayi class banaye!
    Car* mySedan = new Sedan(petrolEng);    // Sedan + Petrol combo
    Car* mySUV = new SUV(electricEng);      // SUV + Electric combo
    Car* yourSUV = new SUV(dieselEng);      // SUV + Diesel combo — same class!

    // STEP 3: Use karo — har drive() apne injected engine ko start karega
    mySedan->drive();   // -> Petrol ignition + Sedan highway
    mySUV->drive();     // -> Electric silent + SUV off-road
    yourSUV->drive();   // -> Diesel roar + SUV off-road

    // STEP 4: Cleanup — demo me intentionally commented (par production
    // me ya to ye deletes chahiye, ya better: unique_ptr/shared_ptr!
    // Dhyan do: engines SHARED nahi hain yahan, har car ka apna hai —
    // agar share karte to shared_ptr sahi choice hota.)
    // delete mySedan;
    // delete mySUV;
    // delete yourSUV;
    // delete petrolEng;
    // delete dieselEng;
    // delete electricEng;

    return 0;
}

// ============================================================================
//  EXPECTED OUTPUT:
// ----------------------------------------------------------------------------
//  Petrol engine starting with ignition!
//  Driving a Sedan on the highway.
//  Electric engine powering up silently!
//  Driving an SUV off-road.
//  Diesel engine roaring to life!
//  Driving an SUV off-road.
//
//  Dekho: SUV ki EK hi class hai, par do alag engines ke saath do alag
//  behaviors aaye — bina SUVElectric/SUVDiesel jaisi classes banaye.
//  2 cars + 3 engines = 5 classes, 6 combos. Bridge ka pura point yahi! ✅
// ============================================================================
