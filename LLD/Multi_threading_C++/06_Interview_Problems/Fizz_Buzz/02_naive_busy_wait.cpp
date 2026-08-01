/**
 * 02 — Naive busy-wait: each thread spins until it is their turn.
 * Works but wastes CPU; interview me "why bad?" puchna common hai.
 */
#include <iostream>
#include <string>
#include <mutex>
#include <thread>

using namespace std;

namespace {

constexpr int kN = 15;

mutex print_mtx;
int current = 1;

bool isMyTurn(int value, const char* role) {
    if (string(role) == "fizz") {
        return value % 3 == 0 && value % 5 != 0;
    }
    if (string(role) == "buzz") {
        return value % 5 == 0 && value % 3 != 0;
    }
    if (string(role) == "fizzbuzz") {
        return value % 15 == 0;
    }
    return value % 3 != 0 && value % 5 != 0;
}

void worker(const char* role, const string& token) {
    while (true) {
        bool done = false;
        while (!done) {
            lock_guard<mutex> guard(print_mtx);
            if (current > kN) {
                return;
            }
            if (isMyTurn(current, role)) {
                cout << token;
                if (string(role) == "number") {
                    cout << current;
                }
                cout << ",";
                ++current;
                done = true;
            }
        }
    }
}

}  // namespace

int main() {
    cout << "====== Fizz Buzz — Naive Busy Wait (n=" << kN << ") ======\n";

    thread t_num(worker, "number", "");
    thread t_fizz(worker, "fizz", "fizz");
    thread t_buzz(worker, "buzz", "buzz");
    thread t_fb(worker, "fizzbuzz", "fizzbuzz");

    t_num.join();
    t_fizz.join();
    t_buzz.join();
    t_fb.join();

    cout << "\n(Done — note: threads spun while waiting; use CV/semaphore in production)\n";
    return 0;
}
