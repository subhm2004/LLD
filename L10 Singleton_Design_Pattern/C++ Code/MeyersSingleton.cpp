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

