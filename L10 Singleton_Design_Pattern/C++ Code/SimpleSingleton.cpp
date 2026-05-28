// Singleton ka simple version:
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