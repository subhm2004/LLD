/**
 * 03 — compare_exchange_strong vs compare_exchange_weak
 *
 * weak may spuriously fail (retry loop). strong only fails on real mismatch.
 */
#include <atomic>
#include <iostream>

using namespace std;

int main() {
    cout << "====== 03 Strong vs Weak compare_exchange ======\n\n";

    atomic<int> value{0};

    // weak in a retry loop (typical lock-free pattern)
    int desired = 1;
    int expected = 0;
    int weak_attempts = 0;
    while (!value.compare_exchange_weak(expected, desired, memory_order_acq_rel,
                                        memory_order_relaxed)) {
        ++weak_attempts;
        expected = 0; // reset after spurious or real failure
        if (weak_attempts > 5) {
            break;
        }
    }
    cout << "weak CAS loop: success after " << weak_attempts
         << " extra attempts, value=" << value.load() << "\n";

    // strong: no spurious failure
    value.store(10);
    expected = 10;
    bool strong_ok = value.compare_exchange_strong(expected, 20);
    cout << "strong CAS(10->20): ok=" << strong_ok << ", value=" << value.load() << "\n";

    expected = 10;
    strong_ok = value.compare_exchange_strong(expected, 99);
    cout << "strong CAS(10->99) should fail: ok=" << strong_ok
         << ", value=" << value.load() << ", expected=" << expected << "\n\n";

    cout << "Rule of thumb:\n";
    cout << "  - Loop / lock-free stack head -> weak + retry\n";
    cout << "  - Single-shot update -> strong\n";
    return 0;
}
