/**
 * 03 — Many readers run in parallel; writer waits for all readers to finish.
 */
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

using namespace std;

string document = "initial content";
shared_mutex mtx;
atomic<int> concurrent_reads{0};
atomic<int> max_concurrent_reads{0};

void reader(int id) {
    for (int i = 0; i < 4; ++i) {
        shared_lock<shared_mutex> lock(mtx);
        int now = ++concurrent_reads;
        max_concurrent_reads.store(max(max_concurrent_reads.load(), now));
        cout << "[reader " << id << "] reading... concurrent=" << now << "\n";
        this_thread::sleep_for(chrono::milliseconds(50));
        --concurrent_reads;
    }
}

int main() {
    cout << "====== 03 Many Readers Parallel ======\n\n";

    vector<thread> readers;
    for (int i = 1; i <= 5; ++i) {
        readers.emplace_back(reader, i);
    }
    for (auto& t : readers) {
        t.join();
    }

    cout << "\nMax concurrent reads observed: " << max_concurrent_reads.load()
         << " (should be > 1)\n";
    cout << "Document still: " << document << "\n";
    return 0;
}
