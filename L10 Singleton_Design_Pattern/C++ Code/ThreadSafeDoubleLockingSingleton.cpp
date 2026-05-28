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