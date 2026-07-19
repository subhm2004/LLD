#include <bits/stdc++.h>
#include <memory>
using namespace std;

// Demo 5: RAII (Resource Acquisition Is Initialization) — Exception aane par bhi resources aur memory cleanup ka guaranteed mechanism.

/**
 * @class Resource
 * @brief Ek standard class jo database connection ya external resource wrap karti hai.
 * 
 * RAII standard guidelines ke mutabik: Resource acquisition constructor me aur resource release destructor me honi chahiye.
 */
class Resource {
public:
    explicit Resource(string name) : name_(std::move(name)) {
        cout << "  [Resource] acquired: " << name_ << "\n";
    }
    
    // Destructor scope end hone par (chahe normal flow ho ya exceptional jump) guaranteed execute hota hai.
    ~Resource() { 
        cout << "  [Resource] released: " << name_ << "\n"; 
    }

    void use() const { 
        cout << "  [Resource] using " << name_ << "\n"; 
    }

private:
    string name_;
};

/**
 * @brief Demo function jo heap ya stack memory handle karke exception trigger kar sakti hai.
 * 
 * @param fail bool flag jo trigger decide karta hai.
 */
void mayThrow(bool fail) {
    // Resource object ko stack memory par create kiya hai.
    Resource r("DB connection"); 
    r.use();

    if (fail) {
        // Exception throw hone par stack unwinding shuru hoti hai.
        // Stack unwinding ka matlab hai ki system current stack frame me se local objects
        // (jaise r) ke destructors ko automatically aur safely call karta hai.
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
        cout << "(Notice: Resource destructor still ran safely due to stack unwinding)\n\n";
    }

    cout << "Case B: unique_ptr — same idea for heap\n";
    try {
        // `unique_ptr` smart pointer RAII rule ke hisab se dynamically allocated memory ko wrap karta hai.
        // Agar heap allocation ke baad exception aati hai, toh standard heap ptr pointer leak nahi hota,
        // unique_ptr ka destructor use delete kar deta hai.
        auto ptr = make_unique<Resource>("smart pointer");
        ptr->use();
        throw runtime_error("fail after unique_ptr");
    } catch (const exception &ex) {
        cout << "Caught: " << ex.what() << "\n";
    }

    cout << "\nRule: LLD code me memory leaks se bachne ke liye hamesha unique_ptr / stack objects (RAII) use karein, raw new/delete nahi.\n";

    return 0;
}
