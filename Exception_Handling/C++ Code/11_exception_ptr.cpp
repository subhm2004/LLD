// C++17 — std::exception_ptr: capture exception, rethrow in another context
#include <bits/stdc++.h>
using namespace std;

exception_ptr runTask(bool fail) {
    exception_ptr ep;
    try {
        if (fail) {
            throw runtime_error("task failed in worker");
        }
        cout << "Task OK\n";
    } catch (...) {
        ep = current_exception(); // store active exception
    }
    return ep;
}

void logAndRethrow(const exception_ptr &ep) {
    if (!ep) {
        return;
    }
    try {
        rethrow_exception(ep);
    } catch (const exception &ex) {
        cout << "Logged in main thread: " << ex.what() << "\n";
        throw; // optional rethrow again
    }
}

int main() {
    cout << "=== 11 std::exception_ptr ===\n";

    exception_ptr err = runTask(true);
    try {
        logAndRethrow(err);
    } catch (const exception &ex) {
        cout << "Handled after rethrow: " << ex.what() << "\n";
    }

    cout << "\nSuccess path:\n";
    exception_ptr ok = runTask(false);
    cout << (ok ? "stored error\n" : "no error stored\n");

    return 0;
}
