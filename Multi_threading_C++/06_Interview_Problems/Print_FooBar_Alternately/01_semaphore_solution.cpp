/**
 * 01 — Print FooBar Alternately (LeetCode 1115).
 * fooSem=1, barSem=0 → strict alternation.
 */
#include <iostream>
#include <thread>

#include "PrintFooBar.h"

using namespace std;
using namespace print_foo_bar;

int main() {
    constexpr int kN = 3;
    cout << "====== Print FooBar (LC 1115, n=" << kN << ") ======\n";

    FooBar foobar(kN);

    thread t_foo([&]() { foobar.foo([]() { cout << "foo"; }); });
    thread t_bar([&]() { foobar.bar([]() { cout << "bar"; }); });

    t_foo.join();
    t_bar.join();

    cout << "\nExpected: foobar repeated " << kN << " times\n";
    return 0;
}
