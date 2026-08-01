// ============================================================================
//  19_Object_Pool_Pattern.cpp  —  Object Pool pattern
// ----------------------------------------------------------------------------
//  Object Pool = baar-baar new/delete karne ki jagah, ek pool me objects PEHLE
//  se bana ke rakho aur use karke wapas pool me de do (reuse). Faayda: allocation
//  ki churn aur memory fragmentation kam, performance behtar — un cheezon ke liye
//  jo bahut frequent banti-mitti hain (DB connections, bullets in a game, threads).
// ============================================================================
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <vector>
using namespace std;

class Connection {
    string host;
    bool inUse = false;
public:
    explicit Connection(string h = "localhost") : host(std::move(h)) {
        cout << "  [Connection] constructed host=" << host << endl;
    }
    ~Connection() {
        cout << "  [Connection] destroyed host=" << host << endl;
    }
    void connect(const string& db) {
        cout << "  [Connection] connect to " << db << " via " << host << endl;
    }
    void reset() { inUse = false; }  // return to pool — clear transient state
    void markInUse() { inUse = true; }
    bool isInUse() const { return inUse; }
};

class ConnectionPool {
    vector<unique_ptr<Connection>> storage;  // owns all objects for lifetime of pool
    queue<Connection*> available;

public:
    explicit ConnectionPool(size_t initialSize, string host = "pool-host") {
        storage.reserve(initialSize);
        for (size_t i = 0; i < initialSize; ++i) {
            auto c = make_unique<Connection>(host + "-" + to_string(i));
            available.push(c.get());
            storage.push_back(std::move(c));
        }
        cout << "[Pool] pre-allocated " << initialSize << " connections\n";
    }

    Connection* acquire() {
        if (available.empty()) {
            cout << "[Pool] grow — new Connection (pool exhausted)\n";
            auto c = make_unique<Connection>("overflow");
            Connection* raw = c.get();
            storage.push_back(std::move(c));
            raw->markInUse();
            return raw;
        }
        Connection* c = available.front();
        available.pop();
        c->markInUse();
        cout << "[Pool] acquire → reuse existing object\n";
        return c;
    }

    void release(Connection* c) {
        if (!c) return;
        c->reset();
        available.push(c);
        cout << "[Pool] release → back to pool (NOT deleted)\n";
    }

    size_t availableCount() const { return available.size(); }
    size_t totalCount() const { return storage.size(); }
};

int main() {
    cout << "=== Without pool: new/delete every request ===\n";
    for (int i = 0; i < 3; ++i) {
        auto c = make_unique<Connection>("ephemeral");
        c->connect("orders_db");
    }

    cout << "\n=== With object pool: reuse same instances ===\n";
    ConnectionPool pool(2);
    Connection* a = pool.acquire();
    a->connect("users_db");
    Connection* b = pool.acquire();
    b->connect("inventory_db");
    pool.release(a);
    pool.release(b);

    cout << "\n=== Re-acquire reuses same memory ===\n";
    Connection* c = pool.acquire();
    c->connect("users_db");
    pool.release(c);

    cout << "\nPool stats: available=" << pool.availableCount()
         << " total=" << pool.totalCount() << endl;

    cout << "\nWhen to use: high-frequency create/destroy, expensive ctor, fixed max objects.\n";
    cout << "Trade-off: complexity, reset() must clear state, thread-safety needs locks.\n";
    return 0;
}
