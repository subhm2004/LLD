// ============================================================================
//  03_This_Pointer.cpp  —  `this` pointer
// ----------------------------------------------------------------------------
//  `this` = ek hidden pointer jo HAR non-static member function ko milta hai,
//  aur current object ka address rakhta hai. Use kab: (1) jab parameter aur
//  member ka naam same ho (this->name = name), (2) method se khud object return
//  karne ke liye (return *this -> method chaining/fluent API).
// ============================================================================
#include <iostream>
using namespace std;

class Counter {
    int count;

public:
    Counter() : count(0) {}

    Counter& increment() {
        this->count++;           // explicit this
        return *this;            // chaining
    }

    void setCount(int count) {
        this->count = count;     // param shadows member — this resolves
    }

    int getCount() const { return count; }
};

int main() {
    Counter c;
    c.increment().increment().increment();  // method chaining via return *this
    cout << "Count: " << c.getCount() << "\n";
    c.setCount(10);
    cout << "Count: " << c.getCount() << "\n";
    return 0;
}
