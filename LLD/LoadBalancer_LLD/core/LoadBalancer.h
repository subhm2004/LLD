// ============================================================================
//  core/LoadBalancer.h  —  Poore system ka FACADE (Strategy ka Context bhi)
// ----------------------------------------------------------------------------
//  Ye class hi client ke liye "load balancer" hai. Andar servers ki registry,
//  connection counts, aur ek routing strategy — sab manage karti hai. Bahar se
//  client ko sirf saaf methods dikhte hain: addServer, routeRequest,
//  completeRequest, markServerStatus, setStrategy.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DO PATTERN EK CLASS ME:                                               │
//  │                                                                          │
//  │  1. FACADE  — client ko servers/strategy ki complexity se bachata hai     │
//  │  2. CONTEXT (Strategy pattern) — ek `LoadBalancingStrategy` hold karta    │
//  │     hai aur routing ka faisla usko DELEGATE karta hai                     │
//  │                                                                          │
//  │  Client `routeRequest()` bulata hai; andar strategy server chunti hai,    │
//  │  LoadBalancer us server ka connection count badha ke return kar deta hai. │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ RUNTIME STRATEGY SWITCH — Strategy pattern ka asli payoff             │
//  │                                                                          │
//  │  `setStrategy()` se chalte-chalte routing algorithm badla ja sakta hai    │
//  │  (RoundRobin -> LeastConnections) bina servers ya kisi aur cheez ko chhue.│
//  │  Ye NFR6 ("strategy switchable at runtime") ko seedha pura karta hai.     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ MEMORY: servers `new` se bante hain (raw Server*), destructor unhe    │
//  │     delete karta hai. Isi wajah se COPY/MOVE explicitly DELETE kiye hain: │
//  │       - agar 2 LoadBalancer same Server* share karte -> double free 💥    │
//  │     (Rule of Three/Five: destructor likha to copy/move ka bhi socho.)     │
//  │     Waise bhi `unique_ptr` member LoadBalancer ko implicitly non-copyable │
//  │     bana deta — par yahan EXPLICITLY = delete karke niyat saaf kar di.    │
//  │     💡 Better hota `vector<unique_ptr<Server>>` — tab manual delete ki    │
//  │        zaroorat hi na padti. Yahan raw+delete "interview-simple" rakha.   │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef LOADBALANCER_LLD_CORE_LOADBALANCER_H
#define LOADBALANCER_LLD_CORE_LOADBALANCER_H

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../enums/ServerStatus.h"
#include "../models/Server.h"
#include "../strategies/LoadBalancingStrategy.h"

using namespace std;

namespace load_balancer_lld {

class LoadBalancer {
public:
    // Strategy inject hoti hai (Dependency Injection) — LB usse own karta hai.
    explicit LoadBalancer(unique_ptr<LoadBalancingStrategy> strategy) : strategy_(std::move(strategy)) {}

    // Servers heap pe hain -> destructor unhe saaf karta hai (koi leak na rahe).
    ~LoadBalancer() {
        for (Server *server : servers_) {
            delete server;
        }
    }

    // ⭐ Copy/move BAND — do LB same Server* share karte to double-free (upar note).
    LoadBalancer(const LoadBalancer &) = delete;
    LoadBalancer &operator=(const LoadBalancer &) = delete;
    LoadBalancer(LoadBalancer &&) = delete;
    LoadBalancer &operator=(LoadBalancer &&) = delete;

    // ---- Server onboard karo (duplicate id reject) --------------------------
    void addServer(const string &serverId, const string &host) {
        if (serverById_.count(serverId) != 0) {
            throw runtime_error("Server already exists");
        }
        Server *server = new Server(serverId, host);
        servers_.push_back(server);       // routing list (order = round robin ke kaam aata)
        serverById_[serverId] = server;   // fast lookup by id
    }

    // ---- Health update karo (UP/DOWN) — routing turant adjust ho jaayegi ----
    void markServerStatus(const string &serverId, ServerStatus status) {
        Server *server = getServerOrThrow(serverId);
        server->setStatus(status);
    }

    // ========================================================================
    //  ⭐ ROUTE REQUEST — strategy se server chuno + connection count badhao
    // ========================================================================
    Server *routeRequest() {
        Server *selected = strategy_->selectServer(servers_); // faisla strategy ka
        if (selected == nullptr) {
            // Koi UP server nahi mila (sab DOWN ya list khaali)
            throw runtime_error("No available server to route request");
        }
        // ⭐ Load count badhao — LeastConnections isi pe depend karta hai, aur
        //    stats sahi rehte hain. (Server.h me counter wala note dekho.)
        selected->incrementConnections();
        return selected;
    }

    // ---- Request complete hui -> us server ka load ghatao -------------------
    void completeRequest(const string &serverId) {
        Server *server = getServerOrThrow(serverId);
        server->decrementConnections();
    }

    // ---- ⭐ Runtime pe routing algorithm badlo (Strategy switch) ------------
    void setStrategy(unique_ptr<LoadBalancingStrategy> strategy) { strategy_ = std::move(strategy); }

    void printServerStats() const {
        cout << "\n=== Server Stats ===\n";
        for (const Server *server : servers_) {
            cout << server->getId() << " | " << server->getHost()
                 << " | Status: " << (server->getStatus() == ServerStatus::UP ? "UP" : "DOWN")
                 << " | ActiveConnections: " << server->getActiveConnections() << "\n";
        }
    }

private:
    vector<Server *> servers_;                     // routing order (round robin isi order me ghoomta)
    unordered_map<string, Server *> serverById_;   // O(1) lookup by id (dono raw ptr, ek hi object)
    unique_ptr<LoadBalancingStrategy> strategy_;   // abhi ka routing algorithm (swappable)

    // Server dhoondho — na mile to saaf error (silent nullptr nahi = fail-fast).
    Server *getServerOrThrow(const string &serverId) const {
        auto it = serverById_.find(serverId);
        if (it == serverById_.end()) {
            throw runtime_error("Server not found: " + serverId);
        }
        return it->second;
    }
};

} // namespace load_balancer_lld

#endif // LOADBALANCER_LLD_CORE_LOADBALANCER_H
