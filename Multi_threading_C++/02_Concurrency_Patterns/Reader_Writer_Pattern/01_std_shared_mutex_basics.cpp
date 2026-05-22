/**
 * 01 — C++17 std::shared_mutex: shared lock for readers, exclusive for writers.
 */
#include <chrono>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>

using namespace std;

int shared_value = 0;
shared_mutex rw_mtx;

void reader(int id) {
    for (int i = 0; i < 3; ++i) {
        shared_lock<shared_mutex> lock(rw_mtx);
        cout << "[reader " << id << "] read value=" << shared_value << "\n";
        this_thread::sleep_for(chrono::milliseconds(80));
    }
}

void writer(int id) {
    for (int i = 0; i < 2; ++i) {
        unique_lock<shared_mutex> lock(rw_mtx);
        ++shared_value;
        cout << "[writer " << id << "] wrote value=" << shared_value << "\n";
        this_thread::sleep_for(chrono::milliseconds(150));
    }
}

int main() {
    cout << "====== 01 std::shared_mutex Basics ======\n\n";

    thread r1(reader, 1);
    thread r2(reader, 2);
    thread w(writer, 1);

    r1.join();
    r2.join();
    w.join();

    cout << "\nFinal value=" << shared_value << "\n";
    return 0;
}
