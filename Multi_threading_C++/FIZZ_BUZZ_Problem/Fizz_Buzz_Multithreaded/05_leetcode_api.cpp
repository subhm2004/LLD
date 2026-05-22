/**
 * 05 — LeetCode 411 API: FizzBuzz class with fizz/buzz/fizzbuzz/number methods.
 */
#include <iostream>
#include <thread>

#include "FizzBuzz.h"

using namespace std;
using namespace fizz_buzz_demo;

int main() {
    constexpr int kN = 15;
    cout << "====== Fizz Buzz — LeetCode API (n=" << kN << ") ======\n";

    FizzBuzz fb(kN);

    thread t_fizz([&]() { fb.fizz([]() {}); });
    thread t_buzz([&]() { fb.buzz([]() {}); });
    thread t_fb([&]() { fb.fizzbuzz([]() {}); });
    thread t_num([&]() { fb.number([](int) {}); });

    t_fizz.join();
    t_buzz.join();
    t_fb.join();
    t_num.join();

    cout << "\n";
    return 0;
}
