/**
 * 01 — Coffman's 4 conditions for deadlock (educational, no hang).
 */
#include <iostream>

using namespace std;

int main() {
    cout << "====== Coffman's 4 Conditions for Deadlock ======\n\n";
    cout << "Deadlock tabhi possible jab CHARO conditions ek saath hon:\n\n";

    cout << "1. Mutual Exclusion\n";
    cout << "   Resource ek waqt sirf ek thread (mutex lock).\n\n";

    cout << "2. Hold and Wait\n";
    cout << "   Thread ek lock hold kare, doosre ka wait kare.\n";
    cout << "   Example: T1 holds mtxA, waits for mtxB.\n\n";

    cout << "3. No Preemption\n";
    cout << "   OS/thread lock forcefully nahi cheen sakti.\n\n";

    cout << "4. Circular Wait\n";
    cout << "   T1 -> waits T2's resource, T2 -> waits T1's resource (cycle).\n\n";

    cout << "Break ANY ONE condition -> deadlock impossible.\n";
    cout << "Common fixes: same lock order, std::lock, scoped_lock, try_lock+backoff.\n";
    return 0;
}
