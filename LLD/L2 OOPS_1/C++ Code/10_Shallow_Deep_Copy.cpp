// ============================================================================
//  10_Shallow_Deep_Copy.cpp  —  Shallow copy vs Deep copy
// ----------------------------------------------------------------------------
//  Shallow copy = pointer field ko WAISE HI copy kar diya -> dono objects ek hi
//  memory point karte hain. Khatra: ek delete kare to doosra dangling pointer;
//  double-delete bhi possible. Deep copy = pointer ke peeche ka data bhi NAYA
//  allocate karke copy karo -> dono independent. Raw pointer wali class me
//  copy ctor/assignment me hamesha DEEP copy karo (Rule of Three).
// ============================================================================
#include <iostream>
#include <cstring>
using namespace std;

void shallowConceptDemo() {
    cout << "=== SHALLOW (pointer copy — shared address) ===\n";
    int* p = new int(42);
    int* q = p;  // shallow — same heap block
    cout << "p and q same address? " << (p == q ? "yes" : "no") << "\n";
    delete p;
    // *q now dangling — never use q after delete
    cout << "After delete p, q is dangling (UB if used)\n\n";
}

class DeepString {
    char* data;
    void deepCopy(const char* s) {
        data = new char[strlen(s) + 1];
        strcpy(data, s);
    }
public:
    DeepString(const char* s) { deepCopy(s); cout << "[Deep] ctor\n"; }
    DeepString(const DeepString& o) {
        deepCopy(o.data);
        cout << "[Deep] copy ctor — new buffer\n";
    }
    DeepString& operator=(const DeepString& o) {
        if (this == &o) return *this;
        delete[] data;
        deepCopy(o.data);
        cout << "[Deep] copy assignment\n";
        return *this;
    }
    ~DeepString() { delete[] data; cout << "[Deep] dtor\n"; }
    void print() const { cout << "  " << data << " @ " << (void*)data << "\n"; }
};

int main() {
    shallowConceptDemo();

    cout << "=== DEEP (separate heap per object) ===\n";
    DeepString a("Hello");
    DeepString b = a;
    cout << "a: "; a.print();
    cout << "b: "; b.print();
    cout << "Different addresses → safe independent dtors\n";
    return 0;
}
