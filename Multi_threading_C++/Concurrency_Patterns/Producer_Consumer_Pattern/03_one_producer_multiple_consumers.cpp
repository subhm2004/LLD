/**
 * 03 — One producer, multiple consumers (SPMC).
 */
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

#include "BoundedBuffer.h"

using namespace std;

int main() {
    cout << "====== 03 One Producer Multiple Consumers ======\n\n";

    BoundedBuffer<int> buffer(6);
    atomic<int> total_consumed{0};

    thread producer([&]() {
        for (int i = 1; i <= 12; ++i) {
            buffer.produce(i);
            cout << "[producer] produced " << i << "\n";
            this_thread::sleep_for(chrono::milliseconds(50));
        }
        buffer.signal_shutdown();
        cout << "[producer] shutdown signaled\n";
    });

    auto consumer_fn = [&](int id) {
        while (auto item = buffer.consume()) {
            cout << "[C" << id << "] consumed " << *item << "\n";
            total_consumed.fetch_add(1);
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        cout << "[C" << id << "] exiting\n";
    };

    thread c1(consumer_fn, 1);
    thread c2(consumer_fn, 2);
    thread c3(consumer_fn, 3);

    producer.join();
    c1.join();
    c2.join();
    c3.join();

    cout << "\nSPMC complete. total_consumed=" << total_consumed.load() << "\n";
    return 0;
}
