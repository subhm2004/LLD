/**
 * 02 — Multiple producers, one consumer (MPSC).
 */
#include <chrono>
#include <iostream>
#include <thread>

#include "BoundedBuffer.h"

using namespace std;

int main() {
    cout << "====== 02 Multiple Producers One Consumer ======\n\n";

    BoundedBuffer<int> buffer(4);
    const int items_each = 5;

    auto producer_fn = [&](int id) {
        for (int i = 0; i < items_each; ++i) {
            int item = id * 100 + i;
            buffer.produce(item);
            cout << "[P" << id << "] produced " << item
                 << " (size=" << buffer.size() << ")\n";
            this_thread::sleep_for(chrono::milliseconds(60));
        }
        cout << "[P" << id << "] finished\n";
    };

    thread p1(producer_fn, 1);
    thread p2(producer_fn, 2);

    thread consumer([&]() {
        int count = 0;
        while (auto item = buffer.consume()) {
            cout << "[consumer] got " << *item << "\n";
            ++count;
            this_thread::sleep_for(chrono::milliseconds(80));
        }
        cout << "[consumer] total consumed=" << count << "\n";
    });

    p1.join();
    p2.join();
    buffer.signal_shutdown();
    consumer.join();

    cout << "\nMPSC complete (expected ~" << 2 * items_each << " items).\n";
    return 0;
}
