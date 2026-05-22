/**
 * 04 — Condition variable + predicate (recommended interview answer).
 */
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

#include "FizzBuzz.h"

using namespace std;
using namespace fizz_buzz_demo;

namespace {

constexpr int kN = 15;

mutex mtx;
condition_variable cv;
int current = 1;

void numberThread() {
    while (true) {
        bool stop = false;
        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [&]() { return current > kN || turnFor(current) == TurnKind::NUMBER; });
            if (current > kN) {
                return;
            }
            cout << current << ",";
            ++current;
            stop = (current > kN);
        }
        cv.notify_all();
        if (stop) {
            return;
        }
    }
}

void tokenThread(TurnKind kind, const string& token) {
    while (true) {
        bool stop = false;
        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [&]() { return current > kN || turnFor(current) == kind; });
            if (current > kN) {
                return;
            }
            cout << token << ",";
            ++current;
            stop = (current > kN);
        }
        cv.notify_all();
        if (stop) {
            return;
        }
    }
}

}  // namespace

int main() {
    cout << "====== Fizz Buzz — Condition Variable (n=" << kN << ") ======\n";

    thread t_num(numberThread);
    thread t_fizz(tokenThread, TurnKind::FIZZ, "fizz");
    thread t_buzz(tokenThread, TurnKind::BUZZ, "buzz");
    thread t_fb(tokenThread, TurnKind::FIZZBUZZ, "fizzbuzz");

    t_num.join();
    t_fizz.join();
    t_buzz.join();
    t_fb.join();

    cout << "\n";
    return 0;
}
