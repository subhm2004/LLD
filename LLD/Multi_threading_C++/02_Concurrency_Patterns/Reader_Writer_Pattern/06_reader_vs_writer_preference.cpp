/**
 * 06 — Reader-preference vs writer-preference locks (starvation behavior).
 */
#include <chrono>
#include <iostream>
#include <thread>

#include "ReadWriteLock.h"

using namespace std;

template <typename RWLock>
void stress_readers(RWLock& rw, const string& label, bool reader_runs) {
    auto reader = [&](int /*id*/) {
        for (int i = 0; i < 5; ++i) {
            rw.lock_shared();
            this_thread::sleep_for(chrono::milliseconds(40));
            rw.unlock_shared();
        }
    };

    auto writer = [&]() {
        rw.lock();
        cout << "[" << label << "] writer acquired lock\n";
        this_thread::sleep_for(chrono::milliseconds(50));
        rw.unlock();
    };

    thread w(writer);
    this_thread::sleep_for(chrono::milliseconds(10));

    thread readers[3];
    if (reader_runs) {
        for (int i = 0; i < 3; ++i) {
            readers[i] = thread(reader, i);
        }
    }

    w.join();
    for (int i = 0; i < 3; ++i) {
        if (readers[i].joinable()) {
            readers[i].join();
        }
    }
}

int main() {
    cout << "====== 06 Reader vs Writer Preference ======\n\n";

    cout << "--- Reader-preference (ReadWriteLock) ---\n";
    cout << "Writer may wait if readers keep arriving.\n";
    ReadWriteLock reader_pref;
    stress_readers(reader_pref, "reader-pref", true);

    cout << "\n--- Writer-preference (WriterPreferenceRWLock) ---\n";
    cout << "After writer waits, new readers block — writer gets turn sooner.\n";
    WriterPreferenceRWLock writer_pref;
    stress_readers(writer_pref, "writer-pref", true);

    cout << "\nDone. Compare which policy fits your workload.\n";
    return 0;
}
