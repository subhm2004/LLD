// ============================================================================
//  strategies/RoundRobinStrategy.h  —  Round Robin routing (CONCRETE strategy)
// ----------------------------------------------------------------------------
//  "Baari-baari se sabko do." Server 1, phir 2, phir 3, phir wapas 1... — ek
//  circle me ghoomte hue har server ko turn-by-turn request milti hai. Sabse
//  simple aur fair distribution (jab saare servers barabar ho).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `nextIndex_` — "agli baar kahan se shuru karein" ka pointer           │
//  │                                                                          │
//  │  Ye member CALLS ke BEECH yaad rehta hai (state). Har selectServer ke     │
//  │  baad ye agle server pe set ho jaata hai. Isi liye ye strategy STATEFUL   │
//  │  hai (LeastConnections ke ulat, jo har baar fresh calculate karti hai).   │
//  │                                                                          │
//  │  Example (3 servers, sab UP):                                            │
//  │     call 1: nextIndex_=0 -> S1 chuna, nextIndex_=1                       │
//  │     call 2: nextIndex_=1 -> S2 chuna, nextIndex_=2                       │
//  │     call 3: nextIndex_=2 -> S3 chuna, nextIndex_=0  (circle poora)       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ DOWN servers ko SKIP karta hai — ye health-awareness zaroori hai
//     Loop `nextIndex_` se shuru karke poore circle me ghoomta hai aur PEHLA
//     available (UP) server chun leta hai. Agar current index wala DOWN hai,
//     to `+i` se agla try karta hai. Isi liye ek server DOWN hone par bhi
//     routing rukti nahi — bas us server ka turn skip ho jaata hai.
//
//  ⭐ SAARE servers DOWN? -> poora loop chal ke koi UP nahi milta -> nullptr.
//     LoadBalancer isko dekh ke "No available server" error deta hai. Crash nahi.
//
//  💡 `(nextIndex_ + i) % n` — modulo se index circle me wrap hota hai (aakhri
//     ke baad wapas 0 pe). Yahi "round" (gol) robin ka matlab hai.
//
//  ⚠ LIMITATION: Round Robin server ka asli LOAD nahi dekhta — bhale ek server
//     par bhaari requests ho, uska turn phir bhi aayega. Jab requests ka size
//     alag-alag ho, LeastConnections behtar hai. (Isi liye dono strategies hain.)
// ============================================================================
#ifndef LOADBALANCER_LLD_STRATEGIES_ROUNDROBINSTRATEGY_H
#define LOADBALANCER_LLD_STRATEGIES_ROUNDROBINSTRATEGY_H

#include <cstddef>
#include <vector>

#include "LoadBalancingStrategy.h"

using namespace std;

namespace load_balancer_lld {

class RoundRobinStrategy : public LoadBalancingStrategy {
public:
    RoundRobinStrategy() : nextIndex_(0) {}

    Server *selectServer(const vector<Server *> &servers) override {
        if (servers.empty()) {
            return nullptr; // koi server hi nahi
        }

        const size_t n = servers.size();
        // nextIndex_ se shuru karke poora circle (n baar) ghoomo, pehla UP chuno.
        for (size_t i = 0; i < n; ++i) {
            const size_t index = (nextIndex_ + i) % n; // circular index
            if (servers[index]->isAvailable()) {
                nextIndex_ = (index + 1) % n; // agli call is ke AGLE se shuru
                return servers[index];
            }
            // ye DOWN tha -> agla try karo (skip)
        }
        return nullptr; // poora circle ghooma, koi UP nahi mila
    }

private:
    size_t nextIndex_; // ⭐ calls ke beech yaad rehta hai (stateful strategy)
};

} // namespace load_balancer_lld

#endif // LOADBALANCER_LLD_STRATEGIES_ROUNDROBINSTRATEGY_H
