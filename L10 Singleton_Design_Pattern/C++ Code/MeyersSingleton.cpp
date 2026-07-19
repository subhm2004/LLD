// ============================================================================
//  MeyersSingleton.cpp — STEP 6 (FINAL): Meyers Singleton — THE WINNER ✅
// ----------------------------------------------------------------------------
//  Poora magic EK line me:  static Singleton instance;  (getInstance ke andar)
//  C++11 se GUARANTEE hai: function-local static ka initialization
//  THREAD-SAFE hota hai — compiler khud internally locking karta hai.
//  Matlab: LAZY (pehli call pe banega) + THREAD-SAFE + ZERO manual locks
//  + destructor bhi program exit pe khud chalega (new/delete ka jhanjhat nahi)!
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  FINAL SCOREBOARD — L10 ke saare versions ek nazar me:                  │
//  │                                                                          │
//  │   Version        | Lazy? | Thread-safe? | Fast? | Simple? | Verdict     │
//  │   ---------------+-------+--------------+-------+---------+------------ │
//  │   NoSingleton    |  n/a  | n/a          | n/a   | ✅      | ❌ singleton│
//  │                  |       |              |       |         |    hi nahi! │
//  │   Simple         |  ✅   | ❌ RACE!     | ✅    | ✅      | ❌ unsafe   │
//  │   Locking        |  ✅   | ✅           | ❌ har| ✅      | ⚠️ slow     │
//  │                  |       |              | call  |         |             │
//  │                  |       |              | lock  |         |             │
//  │   DCL            |  ✅   | ✅(atomic ke | ✅    | ❌      | ⚠️ tricky   │
//  │                  |       |  saath hi    |       | tricky  |             │
//  │                  |       |  100%)       |       |         |             │
//  │   Eager          |  ❌   | ✅           | ✅    | ✅      | ⚠️ waste    │
//  │                  |       |              |       |         |  possible   │
//  │   MEYERS (ye!)   |  ✅   | ✅ C++11     | ✅    | ✅ 3    | ✅ DEFAULT  │
//  │                  |       |  guarantee   |       | lines!  |    CHOICE   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  Ek aur detail: ye REFERENCE (&) return karta hai, pointer nahi — caller
//  galti se delete nahi kar sakta! Repo me real use: L24 CouponManager +
//  DiscountStrategyManager isi style ke hain.
// ============================================================================
#include <bits/stdc++.h>

using namespace std;

// Meyers Singleton (recommended modern C++ style)
// Key idea:
// - instance is a function-local static object.
// - C++11+ guarantees thread-safe initialization for local statics.
class Singleton {
private:
    Singleton() { cout << "Meyers Singleton Constructor called\n"; }

    // No copy / move: singleton instance duplication blocked.
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
    Singleton(Singleton &&) = delete;
    Singleton &operator=(Singleton &&) = delete;

public:
    static Singleton &getInstance() {
        // First call: object create hota hai.
        // Later calls: same object return hota hai.
        static Singleton instance;
        return instance;
    }

    void sayHello(const string &from) const {
        cout << "Hello from Singleton, called by: " << from << '\n';
    }
};

int main() {
    Singleton &s1 = Singleton::getInstance();
    Singleton &s2 = Singleton::getInstance();

    s1.sayHello("s1");
    s2.sayHello("s2");

    cout << "s1 and s2 same object? " << (&s1 == &s2) << '\n';
    return 0;
}

