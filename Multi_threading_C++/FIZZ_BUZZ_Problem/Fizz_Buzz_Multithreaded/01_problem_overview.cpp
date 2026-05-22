/**
 * 01 — Fizz Buzz Multithreaded: problem statement (no threads).
 * LeetCode 411: 4 threads print 1..n in order with fizz/buzz rules.
 */
#include <iostream>

using namespace std;

int main() {
    cout << "====== Fizz Buzz Multithreaded (Overview) ======\n\n";

    cout << "Goal: Print 1..n using FOUR threads in correct order:\n";
    cout << "  - Thread A: 'fizz'  when i % 3 == 0 && i % 5 != 0\n";
    cout << "  - Thread B: 'buzz'  when i % 5 == 0 && i % 3 != 0\n";
    cout << "  - Thread C: 'fizzbuzz' when i % 15 == 0\n";
    cout << "  - Thread D: number when not divisible by 3 or 5\n\n";

    cout << "Example n=15:\n";
    cout << "1,2,fizz,4,buzz,fizz,7,8,fizz,buzz,11,fizz,13,14,fizzbuzz\n\n";

    cout << "Synchronization challenge:\n";
    cout << "  Only ONE thread may print per step; all must agree on 'current' value.\n\n";

    cout << "Solutions in this folder:\n";
    cout << "  02 — naive busy-wait (spin, high CPU)\n";
    cout << "  03 — semaphores + turn controller\n";
    cout << "  04 — condition_variable + predicate (recommended)\n";
    cout << "  05 — LeetCode API via FizzBuzz.h (same as 04)\n\n";

    return 0;
}
