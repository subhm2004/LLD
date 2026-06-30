// ============================================================================
//  16_Rule_Of_Three_Five_Zero.cpp  —  Rule of Three / Five / Zero
// ----------------------------------------------------------------------------
//  Agar class koi resource (raw pointer, file handle) khud manage karti hai:
//    RULE OF THREE : destructor + copy constructor + copy assignment — teeno do.
//    RULE OF FIVE  : upar wale 3 + move constructor + move assignment (C++11) —
//                    performance ke liye move bhi handle karo.
//    RULE OF ZERO  : sabse behtar — raw resource manage hi mat karo; smart
//                    pointers/containers use karo, to compiler ke defaults theek
//                    hain aur tumhe kuch likhna hi nahi padta.
// ============================================================================
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using namespace std;

// Rule of FIVE (simplified)
class RuleOfFive {
    unique_ptr<int[]> data;
    size_t n;
public:
    explicit RuleOfFive(size_t size) : data(make_unique<int[]>(size)), n(size) {
        cout << "[Five] ctor\n";
    }
    ~RuleOfFive() { cout << "[Five] dtor\n"; }

    RuleOfFive(const RuleOfFive& o) : data(make_unique<int[]>(o.n)), n(o.n) {
        cout << "[Five] copy ctor\n";
    }
    RuleOfFive& operator=(const RuleOfFive& o) {
        if (this == &o) return *this;
        data = make_unique<int[]>(o.n);
        n = o.n;
        cout << "[Five] copy assign\n";
        return *this;
    }
    RuleOfFive(RuleOfFive&&) noexcept = default;
    RuleOfFive& operator=(RuleOfFive&&) noexcept = default;
};

// Rule of ZERO — compiler-generated ops are correct
class RuleOfZero {
    string name;
    vector<int> scores;
public:
    RuleOfZero(string n, vector<int> s) : name(std::move(n)), scores(std::move(s)) {}
    void show() const { cout << "[Zero] " << name << " scores=" << scores.size() << "\n"; }
};

int main() {
    RuleOfFive a(10);
    RuleOfFive b = a;
    RuleOfFive c(5);
    c = std::move(a);

    RuleOfZero z("Aditya", {90, 95});
    z.show();
    return 0;
}
