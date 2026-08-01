// ============================================================================
//  ThreadSafeLockingSingleton.cpp — STEP 3: mutex lagao (SAFE par SLOW)
// ----------------------------------------------------------------------------
//  SimpleSingleton ka race-condition fix: getInstance() me ghuste hi
//  mutex LOCK — ab ek waqt pe sirf EK thread check+create kar sakta hai.
//  Race khatam ✅. lock_guard scope-based hai: function se nikalte hi
//  auto-unlock (exception aaye tab bhi — RAII ka kamaal).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠️ NAYI PROBLEM — performance:                                         │
//  │                                                                          │
//  │   Instance to sirf PEHLI call pe banta hai... par lock HAR call pe      │
//  │   lagta hai! 10 lakh getInstance() calls = 10 lakh lock/unlock,         │
//  │   jabki zaroorat sirf pehli wali me thi. Mutex lock sasta nahi hota —   │
//  │   threads apas me takra ke wait bhi karte hain (contention).            │
//  │                                                                          │
//  │   "Ek baar ke kaam ke liye har baar ka kharcha" — isi ko optimize       │
//  │   karta hai agla version: Double-Checked Locking (DCL).                 │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#include <bits/stdc++.h>
#include <iostream>
#include <mutex>

using namespace std;

// Thread-safe Lazy Singleton (coarse locking):
// Har getInstance() call pe mutex lock hota hai.
// Safe hai, par frequent calls me thoda overhead ho sakta hai.
class Singleton {
private:
  static Singleton *instance;
  static mutex mtx;

  // Constructor private => direct object creation allowed nahi.
  Singleton() { cout << "Singleton Constructor Called!" << endl; }

  // Copy/move explicitly disable:
  // Agar yeh allow rahe to singleton ka duplicate object ban sakta hai.
  Singleton(const Singleton &) = delete;            // copy constructor blocked
  Singleton &operator=(const Singleton &) = delete; // copy assignment blocked
  Singleton(Singleton &&) = delete;                 // move constructor blocked
  Singleton &operator=(Singleton &&) = delete;      // move assignment blocked
  // Destructor private hai: outside delete nahi kar paoge.
  ~Singleton() { cout << "Singleton Destructor called" << endl; }

public:
  static Singleton *getInstance() {
    // lock_guard scope-based lock hai:
    // - constructor me lock
    // - function scope end hote hi auto unlock
    lock_guard<mutex> lock(mtx);

    // Critical section: instance create/check
    if (instance == nullptr) {
      instance = new Singleton();
    }
    return instance;
  }
};

// Static members ki definitions
Singleton *Singleton::instance = nullptr;
mutex Singleton::mtx;

int main() {
  Singleton *s1 = Singleton::getInstance();
  Singleton *s2 = Singleton::getInstance();

  // true(1) => singleton behavior confirm.
  cout << "s1 and s2 are same object? " << (s1 == s2) << endl;
}