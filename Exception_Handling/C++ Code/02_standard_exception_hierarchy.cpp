#include <bits/stdc++.h>
using namespace std;

// Demo 2: std::exception hierarchy — catch parent type se child bhi pakdo

void throwLogicError() { throw logic_error("logic_error from std"); }
void throwOutOfRange() { throw out_of_range("out_of_range from vector-like API"); }
void throwRuntime() { throw runtime_error("runtime_error — common in LLD repos"); }

int main() {
    cout << "=== 02 Standard exception hierarchy ===\n\n";

    auto demo = [](const string &label, function<void()> fn) {
        cout << label << " -> ";
        try {
            fn();
        } catch (const exception &ex) { // base — sab std exceptions inherit
            cout << "caught exception: " << ex.what() << "\n";
        }
    };

    demo("logic_error", throwLogicError);
    demo("out_of_range", throwOutOfRange);
    demo("runtime_error", throwRuntime);

    cout << "\nHierarchy (simplified):\n";
    cout << "  exception\n";
    cout << "    logic_error  -> invalid_argument, out_of_range, ...\n";
    cout << "    runtime_error -> range_error, overflow_error, ...\n";

    return 0;
}
