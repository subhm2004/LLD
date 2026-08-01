// ============================================================================
//  dynamic_cast.cpp  —  dynamic_cast SEEKHO (zero se, detail me)
// ----------------------------------------------------------------------------
//  SIMPLE FUNDA:
//  dynamic_cast ek SAFE tareeka hai base class pointer ko wapas derived
//  class pointer me convert karne ka — RUNTIME pe check ke saath.
//
//  ┌─────────────────────────────────────────────────────────────────┐
//  │  UPCASTING (Derived → Base):    hamesha SAFE, cast ki zaroorat  │
//  │                                 hi nahi — automatic hota hai    │
//  │                                                                 │
//  │      Animal* a = new Dog();     ← ye to seedha chal gaya ✅     │
//  │                                                                 │
//  │  DOWNCASTING (Base → Derived):  RISKY! Base pointer ke andar    │
//  │                                 pata nahi KYA baitha hai        │
//  │                                                                 │
//  │      Dog* d = a;                ← compile ERROR ❌              │
//  │      Dog* d = dynamic_cast<Dog*>(a);  ← safe tareeka ✅         │
//  └─────────────────────────────────────────────────────────────────┘
//
//  DOWNCAST RISKY KYUN HAI?
//  Animal* pointer ke andar Dog bhi ho sakta hai, Cat bhi. Agar andar
//  Cat hai aur tumne zabardasti Dog* maan ke bark() call kar diya —
//  CRASH ya garbage (undefined behavior)!
//
//  dynamic_cast KYA KARTA HAI:
//  RUNTIME pe object ka ASLI type check karta hai:
//    - Agar andar sach me Dog hai  → valid Dog* de deta hai ✅
//    - Agar andar Dog NAHI hai     → nullptr de deta hai (crash nahi!) ✅
//  Tum nullptr check karke safely handle kar lete ho.
//
//  YE KAAM KAISE KARTA HAI (andar ka magic):
//  C++ me isse RTTI kehte hain — "Run-Time Type Information".
//  Jab class me koi VIRTUAL function hota hai, to compiler har object
//  ke saath uski type ki info (vtable ke through) store karta hai.
//  dynamic_cast usi info ko runtime pe padh ke decide karta hai ki
//  cast valid hai ya nahi.
//
//  ⚠️ ISI LIYE RULE: dynamic_cast SIRF POLYMORPHIC class pe chalta hai —
//  matlab base class me KAM SE KAM EK virtual function hona chahiye
//  (virtual destructor bhi chalega). Warna compile error!
//
//  CHAARO CASTS KA QUICK COMPARISON:
//  ┌──────────────────┬──────────────────────────────────────────────┐
//  │ static_cast      │ Compile-time cast, KOI runtime check NAHI.   │
//  │                  │ Fast, par galat downcast pe UB — tumhari     │
//  │                  │ zimmedari ki type sahi ho.                   │
//  │ dynamic_cast     │ Runtime check ke saath SAFE downcast.        │
//  │                  │ Thoda slow (check ka cost), par crash-proof. │
//  │ const_cast       │ const hatane/lagane ke liye (rare use).      │
//  │ reinterpret_cast │ Raw memory ko dusre type ki tarah dekhna —   │
//  │                  │ sabse dangerous, LLD me almost kabhi nahi.   │
//  └──────────────────┴──────────────────────────────────────────────┘
//
//  KAB USE KARE dynamic_cast:
//  ✅ Base pointer mila hai aur PAKKA nahi ki andar kaunsa derived hai
//  ✅ Framework/library se Cloneable*/Shape* jaisa generic pointer aaya
//     aur specific type ke extra methods chahiye
//  ✅ Mixed collection (vector<Animal*>) me se specific type dhundhna
//
//  KAB NA USE KARE (design smell 🚨):
//  ❌ Har jagah if(dynamic_cast<X*>)... ki ladder — iska matlab tumhara
//     polymorphism design weak hai. Virtual function banao, cast nahi!
//  ❌ Jab type 100% pata hai — tab covariant return ya design fix better
//     (jaise humne PrototypePattern.cpp me clone() ka return NPC* karke
//      dynamic_cast ki zaroorat hi khatam kar di thi!)
// ============================================================================
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// ============================================================================
//  SECTION 1: EK CHHOTI SI HIERARCHY — Animal → Dog / Cat
// ----------------------------------------------------------------------------
//  Dog aur Cat dono Animal hain, par dono ke paas apne SPECIAL methods
//  bhi hain jo base me nahi hain — yahi downcast ki zaroorat paida karta hai.
// ============================================================================
class Animal {
public:
    // Common behavior — ye virtual hai to har animal apna sound dega.
    // AUR ye virtual function hi Animal ko POLYMORPHIC banata hai —
    // isi wajah se is hierarchy pe dynamic_cast chal payega!
    virtual void makeSound() {
        cout << "Some generic animal sound...\n";
    }

    // Virtual destructor — base pointer se delete karne ke liye zaroori
    // (ye bhi class ko polymorphic banane ke liye kaafi hota hai)
    virtual ~Animal() {}
};

class Dog : public Animal {
public:
    void makeSound() override {
        cout << "Woof! Woof!\n";
    }

    // SPECIAL method — sirf Dog ke paas hai, Animal ke paas NAHI.
    // Isko call karna hai to Dog* chahiye — Animal* se nahi hoga!
    void fetchBall() {
        cout << "Dog is fetching the ball! 🎾\n";
    }
};

class Cat : public Animal {
public:
    void makeSound() override {
        cout << "Meow!\n";
    }

    // SPECIAL method — sirf Cat ke paas
    void scratchSofa() {
        cout << "Cat is scratching the sofa! 🛋️\n";
    }
};

// ============================================================================
//  SECTION 2: EK FUNCTION JISE BASE POINTER MILTA HAI
// ----------------------------------------------------------------------------
//  Real-world scenario: tumhe function me Animal* mila — pata nahi Dog
//  hai ya Cat. Ab agar Dog ho to fetchBall() karwana hai. Kaise pata
//  kare andar kya hai? — dynamic_cast!
// ============================================================================
void playWithAnimal(Animal* animal) {
    // Common kaam — koi cast nahi chahiye, virtual dispatch khud
    // sahi makeSound() chala dega. (Yahi polymorphism ka normal rasta hai)
    animal->makeSound();

    // ---------------- DYNAMIC_CAST IN ACTION ----------------
    // Syntax: dynamic_cast<TargetType*>(sourcePointer)
    //
    // Runtime pe check hoga:
    //   andar sach me Dog hai?  → valid Dog* milega
    //   andar Dog nahi hai?     → nullptr milega (CRASH NAHI!)
    Dog* dog = dynamic_cast<Dog*>(animal);
    if (dog != nullptr) {
        // Cast SUCCESS — ab Dog ke special methods safely use kar sakte hain
        cout << "  [dynamic_cast SUCCESS — ye Dog hi hai]\n";
        dog->fetchBall();
    } else {
        // Cast FAIL — andar Dog nahi tha. Koi crash nahi, gracefully handle!
        cout << "  [dynamic_cast FAIL — ye Dog nahi hai, ball nahi khelega]\n";
    }

    // Shortcut style (same cheez, compact):
    // if (Cat* cat = dynamic_cast<Cat*>(animal)) { cat->scratchSofa(); }
    Cat* cat = dynamic_cast<Cat*>(animal);
    if (cat) {   // pointer ka null-check — if(cat) == if(cat != nullptr)
        cout << "  [dynamic_cast SUCCESS — ye Cat hai]\n";
        cat->scratchSofa();
    }

    cout << "----------------------------------------\n";
}

// ============================================================================
//  SECTION 3: main() — sab scenarios ek-ek karke dekho
// ============================================================================
int main() {
    cout << "========== SCENARIO 1: Andar Dog hai ==========\n";
    // UPCAST — Dog* se Animal* — automatic, koi cast nahi likhna pada.
    // (Dog IS-A Animal, to base pointer me rakhna hamesha safe hai)
    Animal* a1 = new Dog();
    playWithAnimal(a1);   // Dog cast success hoga, Cat cast fail

    cout << "========== SCENARIO 2: Andar Cat hai ==========\n";
    Animal* a2 = new Cat();
    playWithAnimal(a2);   // Dog cast fail hoga (nullptr), Cat cast success

    // ------------------------------------------------------------------
    // SCENARIO 3: GALAT tareeka bhi dekh lo — static_cast se downcast
    // ------------------------------------------------------------------
    cout << "========== SCENARIO 3: static_cast ka khatra ==========\n";
    // static_cast COMPILE ho jayega — compiler ko lagta hai "tumhe pata
    // hoga bhai". PAR runtime check NAHI hota:
    Dog* wrongDog = static_cast<Dog*>(a2);   // a2 me asal me CAT hai!!
    // wrongDog ab ek "Cat ko Dog samajh raha" pointer hai — ye nullptr
    // NAHI hai, par iska use UNDEFINED BEHAVIOR hai:
    // wrongDog->fetchBall();  // ← UNCOMMENT MAT KARNA — crash/garbage!
    cout << "static_cast ne galat cast pakda hi nahi (nullptr nahi diya)\n";
    cout << "  → wrongDog use karte hi undefined behavior hota! ❌\n";

    // Wahi cheez dynamic_cast se — galti turant pakdi jaati hai:
    Dog* safeDog = dynamic_cast<Dog*>(a2);
    cout << "dynamic_cast ne sach bola: "
         << (safeDog == nullptr ? "nullptr mila — a2 Dog NAHI hai ✅" : "Dog hai")
         << "\n";
    cout << "----------------------------------------\n";

    // ------------------------------------------------------------------
    // SCENARIO 4: Mixed collection — real-world jaisa use case
    // ------------------------------------------------------------------
    cout << "========== SCENARIO 4: Mixed zoo me sirf Dogs dhundo ==========\n";
    // vector me sab Animal* hain — Dog, Cat mile-jule. Sirf dogs se
    // ball khilwana hai:
    vector<Animal*> zoo;
    zoo.push_back(new Dog());
    zoo.push_back(new Cat());
    zoo.push_back(new Dog());

    int dogCount = 0;
    for (Animal* animal : zoo) {
        // Har animal pe try karo — jo Dog niklega wahi khelega
        Dog* d = dynamic_cast<Dog*>(animal);
        if (d) {
            dogCount++;
            d->fetchBall();
        }
    }
    cout << "Zoo me total dogs mile: " << dogCount << "\n";
    cout << "----------------------------------------\n";

    // Cleanup — virtual destructor hai isliye base pointer se delete safe
    delete a1;
    delete a2;
    for (Animal* animal : zoo) {
        delete animal;
    }

    return 0;
}

// ============================================================================
//  EXPECTED OUTPUT:
// ----------------------------------------------------------------------------
//  ========== SCENARIO 1: Andar Dog hai ==========
//  Woof! Woof!
//    [dynamic_cast SUCCESS — ye Dog hi hai]
//  Dog is fetching the ball! 🎾
//  ----------------------------------------
//  ========== SCENARIO 2: Andar Cat hai ==========
//  Meow!
//    [dynamic_cast FAIL — ye Dog nahi hai, ball nahi khelega]
//    [dynamic_cast SUCCESS — ye Cat hai]
//  Cat is scratching the sofa! 🛋️
//  ----------------------------------------
//  ========== SCENARIO 3: static_cast ka khatra ==========
//  static_cast ne galat cast pakda hi nahi (nullptr nahi diya)
//    → wrongDog use karte hi undefined behavior hota! ❌
//  dynamic_cast ne sach bola: nullptr mila — a2 Dog NAHI hai ✅
//  ----------------------------------------
//  ========== SCENARIO 4: Mixed zoo me sirf Dogs dhundo ==========
//  Dog is fetching the ball! 🎾
//  Dog is fetching the ball! 🎾
//  Zoo me total dogs mile: 2
//  ----------------------------------------
//
// ============================================================================
//  YAAD RAKHNE WALI BAATEIN (revision ke liye):
// ----------------------------------------------------------------------------
//  1. Upcast (Derived→Base) = free & safe. Downcast (Base→Derived) = risky.
//  2. dynamic_cast = runtime-checked downcast. Fail hua to POINTER version
//     nullptr deti hai. (Reference version dynamic_cast<Dog&> fail pe
//     std::bad_cast exception phenkti hai — pointer version zyada common.)
//  3. Base class POLYMORPHIC honi chahiye (>= 1 virtual function) —
//     warna dynamic_cast compile hi nahi hoga.
//  4. static_cast fast hai par blind hai; dynamic_cast thoda slow par safe.
//  5. Har jagah dynamic_cast dikhe to design smell — pehle socho kya
//     virtual function ya covariant return se kaam ban sakta hai
//     (PrototypePattern.cpp me humne yahi kiya tha!).
// ============================================================================
