/**
 * 01 — Livelock explained (no threads — educational).
 */
#include <iostream>

using namespace std;

int main() {
    cout << "====== What is Livelock? ======\n\n";

    cout << "DEADLOCK:  threads BLOCKED — CPU wait, zero progress.\n";
    cout << "LIVELOCK:  threads ACTIVE — CPU busy, still zero USEFUL progress.\n\n";

    cout << "Real-life analogy (corridor):\n";
    cout << "  Two people face each other.\n";
    cout << "  Both step LEFT to let other pass.\n";
    cout << "  Still blocking each other — repeat forever.\n";
    cout << "  Both MOVING (active) but nobody passes (no progress).\n\n";

    cout << "In code:\n";
    cout << "  try_lock fails -> release locks -> retry immediately\n";
    cout << "  Both threads polite — keep yielding — loop forever.\n\n";

    cout << "Fixes: random backoff, exponential backoff, thread id jitter.\n";
    cout << "See demos 02 (problem) and 03-05 (fixes).\n";
    return 0;
}
