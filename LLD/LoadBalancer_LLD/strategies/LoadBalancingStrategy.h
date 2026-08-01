// ============================================================================
//  strategies/LoadBalancingStrategy.h  —  STRATEGY PATTERN ka interface
// ----------------------------------------------------------------------------
//  "Agli request kaunse server pe bhejein?" — ye ek BADALNE WALA faisla hai.
//  Round Robin? Least Connections? Weighted? IP Hash? Har algorithm alag. Isi
//  liye is faisle ko ek INTERFACE ke peeche rakh diya — yahi Strategy Pattern.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ STRATEGY PATTERN — "algorithm ko plug-and-play banao"                 │
//  │                                                                          │
//  │  LoadBalancer ek `LoadBalancingStrategy*` rakhta hai — usse pata nahi     │
//  │  andar kaunsa algorithm hai. Wo bas `selectServer(servers)` bulata hai.   │
//  │  Kaunsa server chuna jaayega, wo inject ki gayi concrete strategy decide  │
//  │  karti hai. Aur ye RUNTIME pe badla ja sakta hai (setStrategy) — bina     │
//  │  LoadBalancer ko chhue.                                                   │
//  │                                                                          │
//  │  Load Balancer LLD asal me Strategy pattern ka TEXTBOOK example hai —     │
//  │  "same input (servers), alag-alag output (chuna server), algorithm swap". │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ KYUN? (Why Strategy)
//  > Naya algorithm chahiye (WeightedRoundRobin, IpHashStrategy)? Bas nayi class
//  > banao jo ye interface implement kare — LoadBalancer, Server, main ka ek line
//  > nahi badlega. Yahi OPEN/CLOSED PRINCIPLE hai (NFR2: "extensible for new algorithms").
//
//  ⭐ Strategy ko SERVERS ki list PARAMETER me milti hai (khud store nahi karti)
//     — kyunki servers ka maalik LoadBalancer hai (single source of truth).
//     Strategy sirf "diye gaye servers me se ek chuno" ka kaam karti hai
//     (stateless-ish; RoundRobin bas ek chhota index rakhta hai — neeche dekho).
//
//  ⭐ VIRTUAL DESTRUCTOR zaroori: LoadBalancer `unique_ptr<LoadBalancingStrategy>`
//     me derived object rakhta hai base pointer ke through. Bina virtual destructor
//     ke derived ka cleanup chalta hi nahi -> undefined behavior. `= default`
//     isliye kyunki interface ke paas khud delete karne ko kuch nahi.
// ============================================================================
#ifndef LOADBALANCER_LLD_STRATEGIES_LOADBALANCINGSTRATEGY_H
#define LOADBALANCER_LLD_STRATEGIES_LOADBALANCINGSTRATEGY_H

#include <vector>

#include "../models/Server.h"

using namespace std;

namespace load_balancer_lld {

class LoadBalancingStrategy {
public:
    virtual ~LoadBalancingStrategy() = default;

    // Pure virtual (`= 0`) — har concrete strategy ko ye implement karna HI padega.
    // Diye gaye servers me se ek chuno. Koi eligible server nahi -> nullptr.
    virtual Server *selectServer(const vector<Server *> &servers) = 0;
};

} // namespace load_balancer_lld

#endif // LOADBALANCER_LLD_STRATEGIES_LOADBALANCINGSTRATEGY_H
