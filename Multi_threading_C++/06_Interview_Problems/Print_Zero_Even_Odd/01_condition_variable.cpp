/**
 * 01 — Print Zero Even Odd (LeetCode 1116), n=2 → 0102
 */
#include <iostream>
#include <thread>

#include "PrintZeroEvenOdd.h"

using namespace std;
using namespace print_zero_even_odd;

int main() {
    constexpr int kN = 2;
    cout << "====== Zero Even Odd (LC 1116, n=" << kN << ") ======\n";

    ZeroEvenOdd printer(kN);

    thread t_zero([&]() { printer.zero([](int x) { cout << x; }); });
    thread t_even([&]() { printer.even([](int x) { cout << x; }); });
    thread t_odd([&]() { printer.odd([](int x) { cout << x; }); });

    t_zero.join();
    t_even.join();
    t_odd.join();

    cout << "\nExpected for n=2: 0102\n";
    return 0;
}
