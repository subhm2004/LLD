// C++17 — stack unwinding: destructors run while exception propagates
#include <bits/stdc++.h>
using namespace std;

class Trace {
public:
    explicit Trace(string label) : label_(std::move(label)) {
        cout << "  ctor  " << label_ << "\n";
    }
    ~Trace() { cout << "  dtor  " << label_ << "\n"; }

private:
    string label_;
};

void level3() {
    Trace t3("level3");
    throw runtime_error("boom in level3");
}

void level2() {
    Trace t2("level2");
    level3();
}

void level1() {
    Trace t1("level1");
    level2();
}

int main() {
    cout << "=== 14 Stack unwinding (dtors on the way up) ===\n\n";

    try {
        level1();
    } catch (const exception &ex) {
        cout << "caught at main: " << ex.what() << "\n";
    }

    cout << "\nOrder: dtor level3 -> level2 -> level1 -> catch\n";
    return 0;
}
