/**
 * L2 — inline: suggest compiler to expand at call site (no function call overhead)
 * Defined in header usually. Modern compilers inline even without keyword.
 */
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
