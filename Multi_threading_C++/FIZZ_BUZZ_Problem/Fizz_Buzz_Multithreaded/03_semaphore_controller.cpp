/**
 * 03 — Semaphore-based turn controller (C++17 custom semaphore).
 * Exactly one permit active; after print, release next thread's semaphore.
 */
#include <iostream>
#include <mutex>
#include <thread>

#include "CountingSemaphore.h"

using namespace std;
using namespace fizz_buzz_demo;

namespace {

constexpr int kN = 15;

enum class Turn { NUMBER, FIZZ, BUZZ, FIZZBUZZ };

Turn turnFor(int value) {
    if (value % 15 == 0) {
        return Turn::FIZZBUZZ;
    }
    if (value % 3 == 0) {
        return Turn::FIZZ;
    }
    if (value % 5 == 0) {
        return Turn::BUZZ;
    }
    return Turn::NUMBER;
}

mutex print_mtx;
int current = 1;

CountingSemaphore sem_number(1);
CountingSemaphore sem_fizz(0);
CountingSemaphore sem_buzz(0);
CountingSemaphore sem_fizzbuzz(0);

void releaseNextTurn(int nextValue) {
    if (nextValue > kN) {
        sem_number.release();
        sem_fizz.release();
        sem_buzz.release();
        sem_fizzbuzz.release();
        return;
    }
    switch (turnFor(nextValue)) {
        case Turn::NUMBER:
            sem_number.release();
            break;
        case Turn::FIZZ:
            sem_fizz.release();
            break;
        case Turn::BUZZ:
            sem_buzz.release();
            break;
        case Turn::FIZZBUZZ:
            sem_fizzbuzz.release();
            break;
    }
}

void runWorker(CountingSemaphore& mySem, const string& token, bool printValue) {
    while (true) {
        mySem.acquire();
        lock_guard<mutex> guard(print_mtx);
        if (current > kN) {
            return;
        }

        if (printValue) {
            cout << current << ",";
        } else {
            cout << token << ",";
        }
        ++current;
        releaseNextTurn(current);
    }
}

}  // namespace

int main() {
    cout << "====== Fizz Buzz — Semaphore Controller (n=" << kN << ") ======\n";

    thread t_num(runWorker, ref(sem_number), "", true);
    thread t_fizz(runWorker, ref(sem_fizz), "fizz", false);
    thread t_buzz(runWorker, ref(sem_buzz), "buzz", false);
    thread t_fb(runWorker, ref(sem_fizzbuzz), "fizzbuzz", false);

    t_num.join();
    t_fizz.join();
    t_buzz.join();
    t_fb.join();

    cout << "\n";
    return 0;
}
