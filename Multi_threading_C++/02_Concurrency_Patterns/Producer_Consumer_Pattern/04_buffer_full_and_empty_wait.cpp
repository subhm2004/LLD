/**
 * 04 — Buffer full blocks producer; empty blocks consumer (visible waits).
 */
#include <chrono>
#include <iostream>
#include <thread>

#include "BoundedBuffer.h"

using namespace std;

int main() {
    cout << "====== 04 Buffer Full / Empty Wait ======\n\n";

    BoundedBuffer<int> buffer(3);  // small capacity — blocking obvious

    thread producer([&]() {
        for (int i = 1; i <= 8; ++i) {
            cout << "[producer] trying to produce " << i << "...\n";
            buffer.produce(i);
            cout << "[producer] produced " << i
                 << " (size=" << buffer.size() << "/" << buffer.capacity()
                 << ")\n";
            this_thread::sleep_for(chrono::milliseconds(50));
        }
        buffer.signal_shutdown();
    });

    thread consumer([&]() {
        this_thread::sleep_for(chrono::milliseconds(600));
        cout << "[consumer] starting late — producer should block when full\n\n";
        while (auto item = buffer.consume()) {
            cout << "[consumer] consumed " << *item << "\n";
            this_thread::sleep_for(chrono::milliseconds(400));
        }
    });

    producer.join();
    consumer.join();
    cout << "\nObserve: producer pauses when size hits 3 until consumer pops.\n";
    return 0;
}
