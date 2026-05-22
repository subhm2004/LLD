/**
 * 05 — Graceful shutdown: producers finish, then signal_shutdown, consumers exit.
 */
#include <chrono>
#include <iostream>
#include <thread>

#include "BoundedBuffer.h"

using namespace std;

int main() {
    cout << "====== 05 Graceful Shutdown ======\n\n";

    BoundedBuffer<string> buffer(5);

    thread producer([&]() {
        for (int i = 1; i <= 6; ++i) {
            string msg = "job-" + to_string(i);
            buffer.produce(msg);
            cout << "[producer] enqueued " << msg << "\n";
            this_thread::sleep_for(chrono::milliseconds(80));
        }
        cout << "[producer] all jobs sent, signaling shutdown\n";
        buffer.signal_shutdown();
    });

    thread consumer([&]() {
        while (auto msg = buffer.consume()) {
            cout << "[consumer] processing " << *msg << "\n";
            this_thread::sleep_for(chrono::milliseconds(120));
        }
        cout << "[consumer] drained buffer, clean exit\n";
    });

    producer.join();
    consumer.join();
    cout << "\nShutdown complete. shutdown=" << (buffer.is_shutdown() ? "true" : "false")
         << "\n";
    return 0;
}
