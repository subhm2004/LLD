// ============================================================================
//  ThreadSafeDoubleLockingSingleton.cpp — STEP 4: DCL (fast + safe... mostly)
// ----------------------------------------------------------------------------
//  Locking version ki "har call pe lock" problem ka fix — DO checks:
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │   if (!instance) {                 // CHECK #1 — BINA lock (fast path)  │
//  │       lock_guard<mutex> lock(mtx); // lock sirf zaroorat pe             │
//  │       if (!instance) {             // CHECK #2 — lock ke ANDAR          │
//  │           instance = new Singleton();                                   │
//  │       }                                                                 │
//  │   }                                                                     │
//  │                                                                          │
//  │  CHECK #1 kyun: 99.9% calls me instance BANA hota hai — unhe bina      │
//  │    lock ke turant return milta hai. Lock ka kharcha sirf shuru me!      │
//  │  CHECK #2 kyun: do threads ek saath check #1 paas kar gaye -> dono     │
//  │    lock ki line me. Pehla bana ke nikla; dusra lock paake agar phir     │
//  │    check NA kare to DUSRA object bana dega! Check #2 yahi rokta hai.    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠️ CAVEAT (senior-level point): raw pointer wala DCL 100% correct hone
//  ke liye `std::atomic<Singleton*>` / memory ordering chahiye — warna
//  compiler/CPU instruction REORDERING se ek thread ADHURA-constructed
//  object dekh sakta hai (rare par real bug!). Isi jhanjhat ki wajah se
//  modern C++ me MEYERS version (agli file) hi default choice hai.
//  (Ye DCL real use me bhi dikhega: L24 ke DiscountCoupon.cpp me!)
// ============================================================================
#include <bits/stdc++.h>
#include <iostream>
#include <mutex>

using namespace std;

// Thread-safe Lazy Singleton (Double-Checked Locking):
// Idea:
// 1) Pehla null-check bina lock (fast path)
// 2) Sirf zaroorat par lock lo
// 3) Lock ke andar dubara null-check (race avoid)
class Singleton {
private:
    static Singleton *instance;
    static mutex mtx;

    // Constructor private => outside object creation blocked.
    Singleton() {
        cout << "Singleton Constructor Called!" << endl;
    }

    // Copy/move disable (singleton integrity):
    // Isse ensure hota hai ki single instance ka copy ya move clone na ban sake.
    Singleton(const Singleton &) = delete;            // copy constructor blocked
    Singleton &operator=(const Singleton &) = delete; // copy assignment blocked
    Singleton(Singleton &&) = delete;                 // move constructor blocked
    Singleton &operator=(Singleton &&) = delete;      // move assignment blocked
     // Destructor private hai: outside delete nahi kar paoge.
     ~Singleton() {
        cout << "Singleton Destructor called" << endl;
    }
public:
    static Singleton *getInstance() {
        // First check: agar instance already bana hua hai to lock avoid ho jayega.
        if (instance == nullptr) {
            // Lock tabhi lenge jab pehle check me null mila.
            lock_guard<mutex> lock(mtx);

            // Second check mandatory hai:
            // Ho sakta hai dusri thread ne is lock ke wait me instance bana diya ho.
            if (instance == nullptr) {
                instance = new Singleton();
            }
        }
        return instance;
    }
};

// Static members definitions
Singleton *Singleton::instance = nullptr;
mutex Singleton::mtx;

int main() {
    Singleton *s1 = Singleton::getInstance();
    Singleton *s2 = Singleton::getInstance();

    // true(1) => dono same singleton object ko point karte hain.
    cout << "s1 and s2 are same object? " << (s1 == s2) << endl;
}