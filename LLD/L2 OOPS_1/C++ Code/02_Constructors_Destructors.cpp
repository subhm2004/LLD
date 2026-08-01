// ============================================================================
//  02_Constructors_Destructors.cpp  —  Constructor / Destructor / Copy
// ----------------------------------------------------------------------------
//  Constructor = object bante hi chalता hai (setup/initialize). Destructor =
//  object marte waqt chalता hai (cleanup, jaise memory free). Copy constructor +
//  copy assignment = ek object se doosra copy karte waqt chalte hain.
//  RULE OF THREE: agar class raw pointer manage karti hai, to teeno (destructor,
//  copy ctor, copy assignment) khud sahi se define karo — warna double-delete /
//  shallow-copy bugs aate hain.
// ============================================================================
#include <iostream>
#include <cstring>
using namespace std;

class Buffer {
    char* data;
    size_t len;

public:
    // Default constructor
    Buffer() : data(nullptr), len(0) {
        cout << "[Default ctor]\n";
    }

    // Parameterized constructor
    Buffer(const char* text) {
        len = strlen(text);
        data = new char[len + 1];
        strcpy(data, text);
        cout << "[Parameterized ctor] " << data << "\n";
    }

    // Copy constructor — deep copy
    Buffer(const Buffer& other) {
        len = other.len;
        data = new char[len + 1];
        strcpy(data, other.data);
        cout << "[Copy ctor] " << data << "\n";
    }

    // Copy assignment
    Buffer& operator=(const Buffer& other) {
        if (this == &other) return *this;
        delete[] data;
        len = other.len;
        data = new char[len + 1];
        strcpy(data, other.data);
        cout << "[Copy assignment] " << data << "\n";
        return *this;
    }

    // Destructor
    ~Buffer() {
        cout << "[Destructor] " << (data ? data : "(null)") << "\n";
        delete[] data;
    }

    void print() const { cout << "  value: " << (data ? data : "") << "\n"; }
};

int main() {
    Buffer a("Hello");
    Buffer b = a;        // copy ctor
    Buffer c;
    c = a;               // copy assignment
    a.print();
    b.print();
    return 0;
}
