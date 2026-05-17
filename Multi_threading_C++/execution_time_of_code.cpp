#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

int main() {

    // Start time
    auto start = high_resolution_clock::now();

    // ---------------- YOUR CODE ----------------
    
    long long sum = 0;

    for (int i = 0; i < 1000000; i++) {
        sum += i;
    }

    // -------------------------------------------

    // End time
    auto end = high_resolution_clock::now();

    // Calculate duration
    auto duration = duration_cast<microseconds>(end - start);

    cout << "Execution Time: " 
         << duration.count() 
         << " microseconds" << endl;

    return 0;
}