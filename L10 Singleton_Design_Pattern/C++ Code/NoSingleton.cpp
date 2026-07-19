// ============================================================================
//  NoSingleton.cpp  —  Baseline: SINGLETON ke BINA (problem samajhne ke liye)
// ----------------------------------------------------------------------------
//  Singleton = "class ka poore program me sirf EK object bane, aur sab usi
//               ko access karein" — DB connection, config, logger jaise
//               resources ke liye.
//
//  Ye file problem dikhati hai: normal class = jitne chaaho objects!
//  Do DB-connection objects bane to? Duplicate state, resource conflict,
//  inconsistency — jo cheez EK honi chahiye thi wo bikhar gayi.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  L10 KA SAFAR — 6 files ki evolution story (isi order me padho!):       │
//  │                                                                          │
//  │  1. NoSingleton (ye file)  -> problem: kitne bhi objects ban jaate      │
//  │  2. SimpleSingleton        -> lazy + private ctor... par THREAD-UNSAFE  │
//  │  3. ThreadSafeLocking      -> mutex har call pe... safe par SLOW        │
//  │  4. ThreadSafeDoubleLocking-> DCL: lock sirf pehli baar... par tricky   │
//  │  5. ThreadSafeEager        -> start pe hi bana do... par shayad waste   │
//  │  6. MeyersSingleton        -> static local: lazy+safe+simple ✅ WINNER  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  Output me dono constructors chalenge aur s1 == s2 FALSE aayega —
//  yahi problem hai jo baaki 5 files solve karti hain!
// ============================================================================
#include <bits/stdc++.h>

using namespace std;

// Ye class intentionally "NoSingleton" hai.
// Matlab yaha pe hum multiple objects create kar sakte hain bina restriction ke.
class NoSingleton {
public:
    // Constructor har baar object banne par call hoga.
    // Agar 2 baar new karoge to 2 baar constructor chalega.
    NoSingleton() {
        cout << "NoSingleton Constructor called. New object created." << endl;
    }
};

int main() {
    // Object-1 create
    NoSingleton *s1 = new NoSingleton();

    // Object-2 create (alag memory location)
    NoSingleton *s2 = new NoSingleton();

    // Compare addresses:
    // - true(1) => same object
    // - false(0) => different object
    // NoSingleton case me mostly false aayega.
    cout << "s1 and s2 are same object? " << (s1 == s2) << endl;

    // Manual cleanup (good practice when using new)
    delete s1;
    delete s2;
}