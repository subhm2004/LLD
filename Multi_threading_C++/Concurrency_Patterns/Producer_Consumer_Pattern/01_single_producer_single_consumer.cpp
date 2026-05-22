/**
 * 01 — Single producer, single consumer (SPSC) on bounded buffer.
 */
#include <chrono>
#include <iostream>
#include <thread>

#include "BoundedBuffer.h"

using namespace std;

int main() {
    cout << "====== 01 Single Producer Single Consumer ======\n\n";

    BoundedBuffer<int> buffer(5);

    thread producer([&]() {
        for (int i = 1; i <= 10; ++i) {
            buffer.produce(i);
            cout << "[producer] produced " << i
                 << " (buffer size=" << buffer.size() << ")\n";
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        buffer.signal_shutdown();
        cout << "[producer] done, shutdown signaled\n";
    });

    thread consumer([&]() {
        while (auto item = buffer.consume()) {
            cout << "[consumer] consumed " << *item
                 << " (buffer size=" << buffer.size() << ")\n";
            this_thread::sleep_for(chrono::milliseconds(150));
        }
        cout << "[consumer] exiting (buffer empty + shutdown)\n";
    });

    producer.join();
    consumer.join();
    cout << "\nSPSC complete.\n";
    return 0;
}
