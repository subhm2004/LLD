/**
 * L2 — this pointer: hidden pointer to current object
 */
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
