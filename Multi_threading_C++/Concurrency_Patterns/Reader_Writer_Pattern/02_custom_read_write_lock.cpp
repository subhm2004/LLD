/**
 * 02 — Custom ReadWriteLock.h (reader-preference) — same idea as shared_mutex.
 */
#include <chrono>
#include <iostream>
#include <thread>

#include "ReadWriteLock.h"

using namespace std;

int shared_data = 100;
ReadWriteLock rw;

void reader(int id) {
    for (int i = 0; i < 3; ++i) {
        rw.lock_shared();
        cout << "[reader " << id << "] read data=" << shared_data
             << " (active_readers=" << rw.readers() << ")\n";
        this_thread::sleep_for(chrono::milliseconds(100));
        rw.unlock_shared();
    }
}

void writer() {
    rw.lock();
    shared_data = 999;
    cout << "[writer] updated data=" << shared_data << "\n";
    this_thread::sleep_for(chrono::milliseconds(200));
    rw.unlock();
}

int main() {
    cout << "====== 02 Custom ReadWriteLock ======\n\n";

    thread r1(reader, 1);
    thread r2(reader, 2);
    thread w(writer);

    r1.join();
    r2.join();
    w.join();

    cout << "\nFinal data=" << shared_data << "\n";
    return 0;
}
