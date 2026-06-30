// ============================================================================
//  MeyersSingleton.cpp  —  Meyers Singleton (modern, RECOMMENDED)
// ----------------------------------------------------------------------------
//  Sabse saaf tareeka: getInstance() ke andar ek `static` LOCAL variable rakho
//  aur uska reference return karo. C++11+ guarantee karta hai ki function-local
//  static ka initialization THREAD-SAFE hai (compiler khud lock laga deta hai).
//  Yani lazy + thread-safe + zero manual locking. Interview/production ka default.
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

