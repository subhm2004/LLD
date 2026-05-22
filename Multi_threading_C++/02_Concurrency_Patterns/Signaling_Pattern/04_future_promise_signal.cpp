/**
 * 04 — future/promise: one-shot signal that async work finished + result ready.
 */
#include <chrono>
#include <future>
#include <iostream>
#include <thread>

using namespace std;

namespace signaling_demo {

int heavy_computation(int n) {
    this_thread::sleep_for(chrono::seconds(1));
    return n * n;
}

}  // namespace signaling_demo

int main() {
    using namespace signaling_demo;

    cout << "[main] launching async work, doing other tasks...\n";

    promise<int> prom;
    future<int> fut = prom.get_future();

    thread worker([&prom]() {
        int result = heavy_computation(42);
        prom.set_value(result);  // signal + deliver result
        cout << "[worker] result computed, promise set\n";
    });

    for (int i = 0; i < 3; ++i) {
        cout << "[main] other work step " << i << "...\n";
        this_thread::sleep_for(chrono::milliseconds(300));
    }

    cout << "[main] waiting on future (blocks until signal)...\n";
    int value = fut.get();
    cout << "[main] received signal via future: result = " << value << "\n";

    worker.join();
    return 0;
}
