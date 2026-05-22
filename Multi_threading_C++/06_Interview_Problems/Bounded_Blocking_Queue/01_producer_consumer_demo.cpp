/**
 * 01 — Bounded blocking queue (capacity=3), 1 producer, 1 consumer.
 */
#include <chrono>
#include <iostream>
#include <thread>

#include "BoundedBlockingQueue.h"

using namespace std;
using namespace bounded_queue;

int main() {
    cout << "====== Bounded Blocking Queue (capacity=3) ======\n";

    BoundedBlockingQueue<int> queue(3);

    thread producer([&]() {
        for (int i = 1; i <= 8; ++i) {
            queue.enqueue(i);
            cout << "[producer] enqueued " << i << " (size=" << queue.size() << ")\n";
            this_thread::sleep_for(chrono::milliseconds(80));
        }
        cout << "[producer] done\n";
    });

    thread consumer([&]() {
        for (int i = 0; i < 8; ++i) {
            int v = queue.dequeue();
            cout << "[consumer] dequeued " << v << " (size=" << queue.size() << ")\n";
            this_thread::sleep_for(chrono::milliseconds(120));
        }
        cout << "[consumer] done\n";
    });

    producer.join();
    consumer.join();
    cout << "\nProducer blocked when full; consumer blocked when empty.\n";
    return 0;
}
