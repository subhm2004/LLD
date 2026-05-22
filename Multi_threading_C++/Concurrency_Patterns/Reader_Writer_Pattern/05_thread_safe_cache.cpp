/**
 * 05 — Thread-safe cache: frequent reads (shared), rare writes (exclusive).
 */
#include <chrono>
#include <iostream>
#include <map>
#include <optional>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>

using namespace std;

class ThreadSafeCache {
public:
    optional<string> get(const string& key) {
        shared_lock<shared_mutex> lock(mtx_);
        auto it = data_.find(key);
        if (it == data_.end()) {
            return nullopt;
        }
        return it->second;
    }

    void put(const string& key, const string& value) {
        unique_lock<shared_mutex> lock(mtx_);
        data_[key] = value;
    }

    size_t size() const {
        shared_lock<shared_mutex> lock(mtx_);
        return data_.size();
    }

private:
    mutable shared_mutex mtx_;
    map<string, string> data_;
};

int main() {
    cout << "====== 05 Thread-Safe Cache ======\n\n";

    ThreadSafeCache cache;
    cache.put("user:1", "Alice");
    cache.put("user:2", "Bob");

    auto read_worker = [&](int id) {
        for (int i = 0; i < 4; ++i) {
            auto v = cache.get("user:1");
            cout << "[reader " << id << "] user:1="
                 << (v ? *v : string("MISS")) << " size=" << cache.size() << "\n";
            this_thread::sleep_for(chrono::milliseconds(60));
        }
    };

    thread r1(read_worker, 1);
    thread r2(read_worker, 2);
    thread r3(read_worker, 3);

    this_thread::sleep_for(chrono::milliseconds(100));
    cache.put("user:1", "Alice-Updated");
    cout << "[writer] cache updated user:1\n";

    r1.join();
    r2.join();
    r3.join();

    auto final_val = cache.get("user:1");
    cout << "\nFinal user:1=" << (final_val ? *final_val : string("MISS")) << "\n";
    return 0;
}
