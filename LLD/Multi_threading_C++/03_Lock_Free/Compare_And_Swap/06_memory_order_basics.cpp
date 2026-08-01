/**
 * 06 — memory_order basics (relaxed vs acquire-release vs seq_cst).
 */
#include <atomic>
#include <iostream>
#include <thread>

using namespace std;

atomic<bool> ready{false};
atomic<int> payload{0};

void producer() {
    payload.store(42, memory_order_relaxed);
    ready.store(true, memory_order_release); // release pairs with acquire
}

void consumer() {
    while (!ready.load(memory_order_acquire)) {
        // spin
    }
    cout << "  consumer sees payload=" << payload.load(memory_order_relaxed) << "\n";
}

int main() {
    cout << "====== 06 Memory Order Basics ======\n\n";

    thread p(producer);
    thread c(consumer);
    p.join();
    c.join();

    cout << "\nOrders (simplified):\n";
    cout << "  memory_order_relaxed — atomicity only, no cross-thread ordering\n";
    cout << "  acquire / release    — synchronize producer -> consumer\n";
    cout << "  seq_cst              — default; global total order (easiest, slightly costlier)\n";
    cout << "\nDefault compare_exchange uses memory_order_seq_cst unless you pass orders.\n";
    return 0;
}
