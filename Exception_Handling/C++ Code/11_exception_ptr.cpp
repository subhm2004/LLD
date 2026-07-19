// C++17 — std::exception_ptr: Capture exceptions inside a thread/context and rethrow/process them in another thread/context
#include <bits/stdc++.h>
using namespace std;

/**
 * @brief Thread ya asynchronous task block ko mock karne wala handler.
 * 
 * Multithreaded systems me agar worker thread par crash hota hai, toh hum chahte hain ki
 * exception main thread ya coordinator thread par check aur handle ho. 
 * Iske liye C++ `std::exception_ptr` support karta hai.
 * 
 * @param fail decide triggers.
 * @return exception_ptr Pointer to the captured exception object (or nullptr if no error).
 */
exception_ptr runTask(bool fail) {
    exception_ptr ep = nullptr; // initialize empty exception pointer
    try {
        if (fail) {
            throw runtime_error("Task failed in asynchronous worker thread context.");
        }
        cout << "Task completed normally without errors.\n";
    } 
    // catch-all block jo har exception type pakadta hai.
    catch (...) {
        // current_exception() active exception object ko capture karke shared pointer state dynamic pointer return karta hai.
        ep = current_exception(); 
    }
    return ep; // exception reference safely transfer ho jayega caller thread ko.
}

/**
 * @brief Capture kiya exception pointer parse aur handle karne ke liye logger helper.
 */
void logAndRethrow(const exception_ptr &ep) {
    if (!ep) {
        return; // Empty pointer means no error occurred.
    }
    try {
        // rethrow_exception: `std::exception_ptr` ke andar ki exception ko wapas active stack throw dynamic state me trigger karta hai.
        rethrow_exception(ep);
    } 
    catch (const exception &ex) {
        cout << "Logged in main thread (Coordinator layer): " << ex.what() << "\n";
        throw; // optional rethrow again upstream.
    }
}

int main() {
    cout << "=== 11 std::exception_ptr ===\n";

    // Asynchronous task failed case study.
    exception_ptr err = runTask(true);
    try {
        logAndRethrow(err);
    } 
    catch (const exception &ex) {
        cout << "Handled after rethrow in outer block: " << ex.what() << "\n";
    }

    cout << "\nSuccess path execution:\n";
    exception_ptr ok = runTask(false);
    cout << (ok ? "Stored error found.\n" : "No error stored (nullptr pointer).\n");

    return 0;
}
