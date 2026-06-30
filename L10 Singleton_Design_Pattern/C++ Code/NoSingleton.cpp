// ============================================================================
//  NoSingleton.cpp  —  Baseline: Singleton ke BINA (problem dikhane ke liye)
// ----------------------------------------------------------------------------
//  Yahan class normal hai -> jitne chaaho objects ban jaate hain. Agar resource
//  ek hi hona chahiye (jaise ek hi DB connection / config), to multiple objects
//  banna problem hai (duplicate state, resource conflict). Iska fix Singleton —
//  baaki files (Simple/Eager/Locking/DCL/Meyers) progressively better solution.
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