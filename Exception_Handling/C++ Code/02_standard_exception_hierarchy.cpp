#include <bits/stdc++.h>
using namespace std;

// Demo 2: std::exception hierarchy — catch parent type se child ko catch karna

// Kuch logical errors throw karne ke liye helper functions.
void throwLogicError() { 
    // logic_error: Ye compiler ko batata hai ki logic me koi gadbadi hai.
    throw logic_error("logic_error from std"); 
}

void throwOutOfRange() { 
    // out_of_range: out of bounds indices ke liye use hota hai (jaise vector size overflow check).
    throw out_of_range("out_of_range from vector-like API"); 
}

void throwRuntime() { 
    // runtime_error: Aise errors jo compile-time par predict nahi kiye ja sakte, runtime pe aate hain.
    throw runtime_error("runtime_error — common in LLD repos"); 
}

int main() {
    cout << "=== 02 Standard exception hierarchy ===\n\n";

    // Lambda definition jo exception catching flow verify karti hai.
    auto demo = [](const string &label, function<void()> fn) {
        cout << label << " -> ";
        try {
            fn();
        } 
        // IMPORTANT: Yahan hum parent (std::exception) pakad rahe hain.
        // C++ inheritance rules ke mutabik, parent reference sabhi child exceptions (logic_error, out_of_range, runtime_error) ko successfully match and catch kar leta hai (Polymorphic catching).
        catch (const exception &ex) { 
            cout << "caught exception: " << ex.what() << "\n";
        }
    };

    // Alag-alag exceptions trigger karke run time behavior test karenge.
    demo("logic_error", throwLogicError);
    demo("out_of_range", throwOutOfRange);
    demo("runtime_error", throwRuntime);

    cout << "\nHierarchy (simplified):\n";
    cout << "  exception (Base Class)\n";
    cout << "    |-- logic_error (Errors checkable before runtime) -> invalid_argument, out_of_range, ...\n";
    cout << "    `-- runtime_error (Errors unpredictable before runtime) -> range_error, overflow_error, ...\n";

    return 0;
}
