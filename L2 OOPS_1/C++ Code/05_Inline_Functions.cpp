// ============================================================================
//  05_Inline_Functions.cpp  —  Inline functions
// ----------------------------------------------------------------------------
//  `inline` = compiler ko HINT ki function call ki jagah uska code waheen
//  expand kar do -> function-call ka overhead bachta hai (chhote functions ke
//  liye useful). Aksar header me define karte hain. Note: modern compilers
//  khud bhi inline kar dete hain bina keyword ke — yeh sirf ek suggestion hai.
// ============================================================================
#include <iostream>
using namespace std;

class MathUtil {
public:
    // Inline inside class = implicitly inline
    inline int square(int x) const { return x * x; }
};

// Inline outside class — must be in same translation unit
inline int add(int a, int b) { return a + b; }

int main() {
    MathUtil m;
    cout << "square(5) = " << m.square(5) << "\n";
    cout << "add(3,4) = " << add(3, 4) << "\n";
    return 0;
}
