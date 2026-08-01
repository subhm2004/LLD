// ============================================================================
//  strategies/LeastConnectionsStrategy.h  —  Least Connections (CONCRETE strategy)
// ----------------------------------------------------------------------------
//  "Jo sabse KHAALI hai usko do." Har UP server ki active connections dekho, aur
//  jiske sabse KAM chal rahi hain usko agli request bhejo. Isse load naturally
//  balance hota hai — bhaari server ko aur bojh nahi milta.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ ROUND ROBIN se behtar KAB? — jab requests ka "weight" alag ho         │
//  │                                                                          │
//  │  Round Robin sabko baari-baari deta hai, chahe koi server bhaari request  │
//  │  me phasa ho. Least Connections ASLI LOAD dekhta hai:                     │
//  │     S1: 5 connections, S2: 1 connection, S3: 3 connections                │
//  │     -> agli request S2 ko jaayegi (sabse kam bhara hua)                   │
//  │  Isliye jab kuch requests lambi/bhaari hoti hain, ye zyada fair hai.      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ STATELESS strategy — RoundRobin ke ULT                                    │
//     Iske paas koi member variable nahi. Har call pe servers ko FRESH scan
//     karke min nikaalta hai. Load ki "state" servers ke paas hai (unke
//     activeConnections_ me), strategy ke paas nahi. Isi liye ye pure function
//     jaisi hai — same input -> same output.
//
//  💡 `numeric_limits<int>::max()` se shuru karte hain (sabse bada possible int)
//     — taaki pehla hi available server isse chhota nikle aur `best` ban jaaye.
//     Ye "abhi tak ka minimum" ko initialize karne ka classic trick hai.
//
//  ⚠ TIE-BREAK: agar do server ki connections barabar ho, to `<` (strict) hone
//     ki wajah se PEHLA (list me jo pehle aata) chuna jaata hai — deterministic.
//     (Barabar wale ko replace nahi karta, isliye first-wins.)
//
//  ⚠ Ye O(n) scan hai (har server dekhta). Bahut zyada servers ho to ek min-heap
//     se O(log n) ho sakta — par uski complexity yahan zaroorat se zyada hai.
// ============================================================================
#ifndef LOADBALANCER_LLD_STRATEGIES_LEASTCONNECTIONSSTRATEGY_H
#define LOADBALANCER_LLD_STRATEGIES_LEASTCONNECTIONSSTRATEGY_H

#include <limits>
#include <vector>

#include "LoadBalancingStrategy.h"

using namespace std;

namespace load_balancer_lld {

class LeastConnectionsStrategy : public LoadBalancingStrategy {
public:
    Server *selectServer(const vector<Server *> &servers) override {
        Server *best = nullptr;
        int minConnections = numeric_limits<int>::max(); // "abhi tak ka min" — sabse bade se shuru

        for (Server *server : servers) {
            if (!server->isAvailable()) {
                continue; // DOWN server skip
            }
            // Strict `<` -> sirf tabhi replace jab STRICTLY kam ho (tie -> first wins)
            if (server->getActiveConnections() < minConnections) {
                minConnections = server->getActiveConnections();
                best = server;
            }
        }
        return best; // koi UP server nahi mila to nullptr
    }
};

} // namespace load_balancer_lld

#endif // LOADBALANCER_LLD_STRATEGIES_LEASTCONNECTIONSSTRATEGY_H
