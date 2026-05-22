/**
 * 06 — Manual implementation (no BoundedBuffer class) — dual CV like interview code.
 */
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

using namespace std;

constexpr size_t kCapacity = 4;

queue<int> buffer;
mutex mtx;
condition_variable cv_not_full;
condition_variable cv_not_empty;
bool done = false;

void producer(int id) {
    for (int i = 0; i < 6; ++i) {
        unique_lock<mutex> lock(mtx);
        cv_not_full.wait(lock, [] {
            return buffer.size() < kCapacity;
        });
        int item = id * 10 + i;
        buffer.push(item);
        cout << "[P" << id << "] push " << item
             << " size=" << buffer.size() << "\n";
        lock.unlock();
        cv_not_empty.notify_one();
        this_thread::sleep_for(chrono::milliseconds(70));
    }
}

void consumer() {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv_not_empty.wait(lock, [] {
            return !buffer.empty() || done;
        });
        if (buffer.empty() && done) {
            break;
        }
        int item = buffer.front();
        buffer.pop();
        cout << "[consumer] pop " << item
             << " size=" << buffer.size() << "\n";
        lock.unlock();
        cv_not_full.notify_one();
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

int main() {
    cout << "====== 06 Manual Dual Condition Variable ======\n\n";

    thread p(producer, 1);
    thread c(consumer);

    p.join();
    {
        lock_guard<mutex> lock(mtx);
        done = true;
    }
    cv_not_empty.notify_all();
    c.join();

    cout << "\nManual producer-consumer complete.\n";
    return 0;
}
