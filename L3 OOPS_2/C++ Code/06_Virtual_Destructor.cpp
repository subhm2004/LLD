/**
 * Virtual destructor — mandatory when deleting derived via base pointer
 */
#include <iostream>
using namespace std;

class BadBase {
public:
    ~BadBase() { cout << "~BadBase (NON-virtual)\n"; }
};

class DerivedBad : public BadBase {
    int* heapData;
public:
    DerivedBad() : heapData(new int[100]) { cout << "DerivedBad ctor\n"; }
    ~DerivedBad() {
        delete[] heapData;
        cout << "~DerivedBad (freed heap)\n";
    }
};

class GoodBase {
public:
    virtual ~GoodBase() { cout << "~GoodBase (virtual)\n"; }
};

class DerivedGood : public GoodBase {
    int* heapData;
public:
    DerivedGood() : heapData(new int[100]) { cout << "DerivedGood ctor\n"; }
    ~DerivedGood() {
        delete[] heapData;
        cout << "~DerivedGood (freed heap)\n";
    }
};

int main() {
    cout << "=== delete via base* WITHOUT virtual dtor ===\n";
    BadBase* p1 = new DerivedBad();
    delete p1;  // Often only ~BadBase — ~DerivedBad SKIPPED → LEAK

    cout << "\n=== delete via base* WITH virtual dtor ===\n";
    GoodBase* p2 = new DerivedGood();
    delete p2;  // ~DerivedGood then ~GoodBase — correct

    cout << "\nRule: polymorphic base → always virtual ~Base()\n";
    return 0;
}
