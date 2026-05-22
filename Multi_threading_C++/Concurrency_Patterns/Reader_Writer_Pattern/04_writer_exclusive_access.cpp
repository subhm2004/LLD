/**
 * 04 — Writer has exclusive access: no reader during write.
 */
#include <chrono>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>

using namespace std;

int counter = 0;
shared_mutex mtx;

void reader(int id) {
    for (int i = 0; i < 5; ++i) {
        shared_lock<shared_mutex> lock(mtx);
        int snap = counter;
        cout << "[reader " << id << "] snapshot counter=" << snap << "\n";
        this_thread::sleep_for(chrono::milliseconds(30));
        if (counter != snap) {
            cout << "[reader " << id << "] ERROR: value changed during read!\n";
        }
    }
}

void writer() {
    for (int i = 0; i < 3; ++i) {
        unique_lock<shared_mutex> lock(mtx);
        cout << "[writer] --- exclusive section start ---\n";
        int old = counter;
        this_thread::sleep_for(chrono::milliseconds(200));
        counter = old + 10;
        cout << "[writer] --- exclusive section end, counter=" << counter
             << " ---\n";
    }
}

int main() {
    cout << "====== 04 Writer Exclusive Access ======\n\n";

    thread w(writer);
    thread r1(reader, 1);
    thread r2(reader, 2);

    w.join();
    r1.join();
    r2.join();

    cout << "\nFinal counter=" << counter << " (no torn reads if locks correct)\n";
    return 0;
}
