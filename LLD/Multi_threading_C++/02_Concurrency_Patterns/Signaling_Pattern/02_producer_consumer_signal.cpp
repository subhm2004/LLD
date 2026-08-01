/**
 * 02 — Producer-Consumer via signaling (two condition variables).
 * Producer signals: item available. Consumer signals: slot free.
 */
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

using namespace std;

namespace signaling_demo {

constexpr int kCapacity = 5;
constexpr int kItemsPerProducer = 8;

queue<int> buffer;
mutex mtx;
condition_variable cv_not_full;
condition_variable cv_not_empty;
bool producers_done = false;

void producer(int id) {
    for (int i = 0; i < kItemsPerProducer; ++i) {
        unique_lock<mutex> lock(mtx);
        cv_not_full.wait(lock, [] {
            return buffer.size() < static_cast<size_t>(kCapacity);
        });

        int item = id * 100 + i;
        buffer.push(item);
        cout << "[P" << id << "] produced " << item
             << " (size=" << buffer.size() << ")\n";
        lock.unlock();
        cv_not_empty.notify_one();
        this_thread::sleep_for(chrono::milliseconds(80));
    }
}

void consumer(int id) {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv_not_empty.wait(lock, [] {
            return !buffer.empty() || producers_done;
        });

        if (buffer.empty() && producers_done) {
            break;
        }

        int item = buffer.front();
        buffer.pop();
        cout << "[C" << id << "] consumed " << item
             << " (size=" << buffer.size() << ")\n";
        lock.unlock();
        cv_not_full.notify_one();
        this_thread::sleep_for(chrono::milliseconds(120));
    }
}

}  // namespace signaling_demo

int main() {
    using namespace signaling_demo;

    thread p1(producer, 1);
    thread p2(producer, 2);
    thread c(consumer, 1);

    p1.join();
    p2.join();

    {
        lock_guard<mutex> lock(mtx);
        producers_done = true;
    }
    cv_not_empty.notify_all();

    c.join();
    cout << "Producer-Consumer signaling complete.\n";
    return 0;
}
