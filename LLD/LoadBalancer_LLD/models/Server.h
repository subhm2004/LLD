// ============================================================================
//  models/Server.h  —  Ek backend server (jispe requests bhejte hain)
// ----------------------------------------------------------------------------
//  Har server ke paas: ek unique id, host address, health status (UP/DOWN), aur
//  abhi kitni active connections chal rahi hain (uska "load").
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `activeConnections_` — LeastConnections strategy ka poora aadhaar     │
//  │                                                                          │
//  │  Jab request is server pe route hoti hai -> incrementConnections() (load  │
//  │  badha). Jab wo request complete ho jaati hai -> decrementConnections()   │
//  │  (load ghata). LeastConnectionsStrategy isi number ko dekh ke sabse "kam  │
//  │  bhara hua" server chunti hai. Yaani ye counter accurate rehna ZAROORI    │
//  │  hai — warna load balancing hi galat ho jaayegi.                         │
//  │                                                                          │
//  │  ⚠ Isi liye har routeRequest() ke saath eventual completeRequest() hona   │
//  │  chahiye — warna count sirf badhta rahega aur server "hamesha busy" dikhega│
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `decrementConnections()` me GUARD hai (`> 0` check) — count kabhi NEGATIVE
//     nahi ho sakta. Agar galti se ek hi request do baar complete ho jaaye, to
//     bhi count 0 se neeche nahi girega. Chhota invariant, bada bachav.
//
//  ⭐ `isAvailable()` = status UP hai kya. Strategies isi ek helper se health
//     check karti hain — "UP hai to hi bhejo" ka logic ek jagah (DRY).
//
//  📌 Ye ek "rich model" hai — sirf data nahi, thoda behavior (increment/
//     decrement/available) bhi rakhta hai. Kyunki ye behavior server ki apni
//     state (count, status) se juda hai, isliye yahin rehna sahi hai (encapsulation).
// ============================================================================
#ifndef LOADBALANCER_LLD_MODELS_SERVER_H
#define LOADBALANCER_LLD_MODELS_SERVER_H

#include <string>
#include <utility>

#include "../enums/ServerStatus.h"

using namespace std;

namespace load_balancer_lld {

class Server {
public:
    // Naya server hamesha UP se aur 0 connections se shuru hota hai.
    Server(string serverId, string host)
        : serverId_(std::move(serverId)),
          host_(std::move(host)),
          status_(ServerStatus::UP),
          activeConnections_(0) {}

    // ---- Getters ------------------------------------------------------------
    const string &getId() const { return serverId_; }
    const string &getHost() const { return host_; }
    ServerStatus getStatus() const { return status_; }
    int getActiveConnections() const { return activeConnections_; }

    // ---- Health ------------------------------------------------------------
    bool isAvailable() const { return status_ == ServerStatus::UP; } // route ke liye eligible?
    void setStatus(ServerStatus status) { status_ = status; }

    // ---- Load counters -----------------------------------------------------
    void incrementConnections() { ++activeConnections_; } // request aayi
    void decrementConnections() {
        if (activeConnections_ > 0) { // guard: kabhi negative nahi
            --activeConnections_;
        }
    }

private:
    string serverId_;        // "S1" — unique
    string host_;            // "10.0.0.1:8080"
    ServerStatus status_;    // UP / DOWN
    int activeConnections_;  // abhi kitni requests chal rahi hain (load)
};

} // namespace load_balancer_lld

#endif // LOADBALANCER_LLD_MODELS_SERVER_H
