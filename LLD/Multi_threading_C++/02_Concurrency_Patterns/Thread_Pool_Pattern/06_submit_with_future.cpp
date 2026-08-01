/**
 * 06 — submit() + future: get async result from pool without blocking main until needed.
 */
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "ThreadPool.h"

using namespace std;

int compute_square(int n) {
    this_thread::sleep_for(chrono::milliseconds(200));
    return n * n;
}

int main() {
    cout << "====== 06 Submit with Future ======\n\n";

    ThreadPool pool(3);
    vector<future<int>> futures;

    for (int i = 1; i <= 6; ++i) {
        futures.push_back(pool.submit(compute_square, i));
        cout << "[main] submitted square(" << i << ")\n";
    }

    cout << "\n[main] doing other work while pool runs tasks...\n";
    this_thread::sleep_for(chrono::milliseconds(100));

    cout << "\n[main] collecting results:\n";
    for (size_t i = 0; i < futures.size(); ++i) {
        int result = futures[i].get();
        cout << "  square(" << (i + 1) << ") = " << result << "\n";
    }

    cout << "\nDone.\n";
    return 0;
}
