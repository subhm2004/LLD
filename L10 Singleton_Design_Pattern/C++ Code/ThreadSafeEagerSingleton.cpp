#include <bits/stdc++.h>
#include <iostream>
#include <mutex>

using namespace std;

// Eager Singleton:
// Object program start ke time par hi create kar diya jata hai
// (lazy nahi hota). Isliye thread-safety creation ke time free me mil jati hai.
class Singleton {
private:
  static Singleton *instance;

  // Constructor private => direct object creation blocked.
  Singleton() { cout << "Singleton Constructor Called!" << endl; }

  // Copy/move disable karne ka reason:
  // Singleton object ka accidental duplicate/clone na ban sake.
  Singleton(const Singleton &) = delete;            // copy constructor blocked
  Singleton &operator=(const Singleton &) = delete; // copy assignment blocked
  Singleton(Singleton &&) = delete;                 // move constructor blocked
  Singleton &operator=(Singleton &&) = delete;      // move assignment blocked
 // Destructor private hai: outside delete nahi kar paoge.
 ~Singleton() {
    cout << "Singleton Destructor called" << endl;
}
public:
  // Accessor sirf existing instance return karta hai.
  static Singleton *getInstance() { return instance; }
};

// Eager creation:
// Ye line load hote hi object create kar deti hai (main se pehle).
Singleton *Singleton::instance = new Singleton();

int main() {
  Singleton *s1 = Singleton::getInstance();
  Singleton *s2 = Singleton::getInstance();

  // true(1) => dono same pre-created object ko point karte hain.
  cout << "s1 and s2 are same object? " << (s1 == s2) << endl;
}