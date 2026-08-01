/**
 * 01 — What is Compare-And-Swap (CAS)?
 *
 * CAS atomically: read memory, compare with expected, swap if equal.
 * C++: std::atomic<T>::compare_exchange_strong / compare_exchange_weak
 */
#include <atomic>
#include <iostream>

using namespace std;

int main() {
    cout << "====== 01 What is Compare-And-Swap ======\n\n";

    atomic<int> balance{100};

    cout << "Initial balance: " << balance.load() << "\n\n";

    // Successful CAS: expected matches current (100) -> write 80
    int expected = 100;
    bool ok = balance.compare_exchange_strong(expected, 80);
    cout << "CAS(100 -> 80): success=" << (ok ? "yes" : "no")
         << ", balance=" << balance.load() << ", expected after fail=" << expected << "\n";

    // Failed CAS: we thought balance was 100, but it is 80 now
    expected = 100;
    ok = balance.compare_exchange_strong(expected, 50);
    cout << "CAS(100 -> 50): success=" << (ok ? "yes" : "no")
         << ", balance=" << balance.load()
         << " (expected updated to actual: " << expected << ")\n\n";

    cout << "Interview one-liner:\n";
    cout << "  CAS = read + compare + conditional write in ONE atomic step.\n";
    cout << "  On failure, hardware updates 'expected' to current value for retry.\n";

    return 0;
}
