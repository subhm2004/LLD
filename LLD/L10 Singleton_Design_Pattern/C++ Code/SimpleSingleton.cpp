// ============================================================================
//  SimpleSingleton.cpp  —  STEP 2: Simple LAZY Singleton (thread-UNSAFE!)
// ----------------------------------------------------------------------------
//  Singleton banane ki 3-item recipe (har version me yahi base hai):
//    1. PRIVATE constructor      -> bahar se object banana IMPOSSIBLE
//    2. Static instance pointer  -> class-level ekmatra copy ka address
//    3. Public getInstance()     -> ekmatra darwaza — pehli baar banao
//                                   (LAZY), baad me wahi lautao
//  + copy/move DELETE (4 lines)  -> clone/move se duplicate banne ka
//                                   piche ka darwaza bhi band!
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠️ IS VERSION KA BUG — race condition (interview ka pakka sawal):     │
//  │                                                                          │
//  │   Thread A: if (instance == nullptr)  <- null dikha, andar ghusa        │
//  │   Thread B: if (instance == nullptr)  <- A ne abhi banaya NAHI —        │
//  │                                          B ko BHI null dikha!           │
//  │   Thread A: instance = new Singleton();   <- object #1                  │
//  │   Thread B: instance = new Singleton();   <- object #2 !! 💥            │
//  │                                                                          │
//  │   DO objects ban gaye — singleton toot gaya + pehla wala LEAK ho gaya.  │
//  │   Single-thread me ye kabhi nahi hoga, isliye bug chhupa rehta hai —    │
//  │   production me multi-thread aate hi phat-ta hai! Fix: agli files.      │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
// Goal: class ka sirf ek hi object poori application me exist kare.

#include <bits/stdc++.h>

using namespace std;

class Singleton {
private:
    // Static pointer class-level shared hota hai.
    // Isme singleton object ka address store hota hai.
    static Singleton *instance;

    // Constructor private => class ke bahar se object create nahi ho sakta.
    Singleton() {
        cout << "Singleton Constructor called" << endl;
    }

    // Copy/move disable kar diya taki existing singleton ka clone na ban sake.
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
    Singleton(Singleton &&) = delete;
    Singleton &operator=(Singleton &&) = delete;

    // Destructor private hai: outside delete nahi kar paoge.
    ~Singleton() {
        cout << "Singleton Destructor called" << endl;
    }

public:
    // Single global access point.
    static Singleton *getInstance() {
        // Lazy initialization:
        // pehli call pe object create hoga, baad me same object return hoga.
        if (instance == nullptr) {
            instance = new Singleton();
        }
        return instance;
    }
};

// Static member ki definition class ke bahar likhna zaroori hota hai.
Singleton *Singleton::instance = nullptr;

int main() {
    Singleton *s1 = Singleton::getInstance();
    Singleton *s2 = Singleton::getInstance();

    // true(1) => dono same singleton object ki taraf point kar rahe hain.
    cout << "s1 and s2 are same object? " << (s1 == s2) << endl;
}