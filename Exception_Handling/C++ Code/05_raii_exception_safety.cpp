#include <bits/stdc++.h>
#include <memory>
using namespace std;

// Demo 5: RAII — exception aaye tab bhi destructor / unique_ptr cleanup

class Resource {
public:
    explicit Resource(string name) : name_(std::move(name)) {
        cout << "  [Resource] acquired: " << name_ << "\n";
    }
    ~Resource() { cout << "  [Resource] released: " << name_ << "\n"; }

    void use() const { cout << "  [Resource] using " << name_ << "\n"; }

private:
    string name_;
};

void mayThrow(bool fail) {
    Resource r("DB connection"); // stack — scope end par ~Resource() chalega
    r.use();

    if (fail) {
        throw runtime_error("operation failed after resource acquired");
    }

    cout << "  Operation completed normally\n";
}

int main() {
    cout << "=== 05 RAII & exception safety ===\n\n";

    cout << "Case A: exception after Resource on stack\n";
    try {
        mayThrow(true);
    } catch (const exception &ex) {
        cout << "Caught: " << ex.what() << "\n";
        cout << "(Notice: Resource destructor still ran)\n\n";
    }

    cout << "Case B: unique_ptr — same idea for heap\n";
    try {
        auto ptr = make_unique<Resource>("smart pointer");
        ptr->use();
        throw runtime_error("fail after unique_ptr");
    } catch (const exception &ex) {
        cout << "Caught: " << ex.what() << "\n";
    }

    cout << "\nRule: LLD code me owning memory = unique_ptr / stack objects, not raw new\n";

    return 0;
}
