/**
 * 02 — FooBar with mutex + condition_variable (turn flag).
 */
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

int main() {
    constexpr int kN = 3;
    cout << "====== Print FooBar — mutex + CV ======\n";

    mutex mtx;
    condition_variable cv;
    bool foo_turn = true;

    thread t_foo([&]() {
        for (int i = 0; i < kN; ++i) {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [&]() { return foo_turn; });
            cout << "foo";
            foo_turn = false;
            cv.notify_all();
        }
    });

    thread t_bar([&]() {
        for (int i = 0; i < kN; ++i) {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [&]() { return !foo_turn; });
            cout << "bar";
            foo_turn = true;
            cv.notify_all();
        }
    });

    t_foo.join();
    t_bar.join();
    cout << "\n";
    return 0;
}
