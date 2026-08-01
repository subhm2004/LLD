// ============================================================================
//  11_Operator_Overloading.cpp  —  Operator overloading
// ----------------------------------------------------------------------------
//  Operator overloading = built-in operators (+, ==, <<, ...) ko apni class ke
//  objects ke liye custom meaning dena, taaki code natural lage
//  (jaise c3 = c1 + c2 do Complex numbers ke liye). Yeh COMPILE-TIME
//  polymorphism hai (kaunsa operator chalega compile pe decide hota hai).
//  Dhyaan: operator ka meaning intuitive rakho, warna code confusing ho jaata.
// ============================================================================
#include <iostream>
using namespace std;

class Complex {
    double real, imag;
public:
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}

    // Member overload: c = a + b
    Complex operator+(const Complex& o) const {
        return Complex(real + o.real, imag + o.imag);
    }

    Complex& operator+=(const Complex& o) {
        real += o.real;
        imag += o.imag;
        return *this;
    }

    bool operator==(const Complex& o) const {
        return real == o.real && imag == o.imag;
    }

    friend ostream& operator<<(ostream& os, const Complex& c);
};

ostream& operator<<(ostream& os, const Complex& c) {
    os << c.real << " + " << c.imag << "i";
    return os;
}

int main() {
    Complex a(1, 2), b(3, 4);
    Complex c = a + b;
    cout << "a + b = " << c << "\n";
    a += b;
    cout << "after += : " << a << "\n";
    cout << "a == b ? " << (a == b ? "yes" : "no") << "\n";
    return 0;
}
