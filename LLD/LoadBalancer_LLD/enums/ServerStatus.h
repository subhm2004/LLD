// ============================================================================
//  enums/ServerStatus.h  —  Server abhi zinda hai ya nahi (health state)
// ----------------------------------------------------------------------------
//     UP   -> server healthy hai, requests le sakta hai
//     DOWN -> server down/unhealthy hai, ispe request mat bhejo
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ye enum LOAD BALANCER ke health-awareness ka dil hai                  │
//  │                                                                          │
//  │  Har routing strategy (RoundRobin / LeastConnections) request bhejne se   │
//  │  pehle `server->isAvailable()` check karti hai — jo internally status ==  │
//  │  UP dekhta hai. DOWN servers ko route SKIP kar deti hain. Isi liye ek     │
//  │  server DOWN mark karte hi traffic apne aap baaki UP servers pe shift     │
//  │  ho jaata hai (self-healing routing).                                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Real load balancer me health "manually" set nahi hoti — periodic health
//     checks (ping/HTTP probe) automatically UP/DOWN toggle karte hain. Yahan
//     demo hai, isliye markServerStatus() se manually simulate karte hain
//     (requirements ki Assumption me bhi likha hai).
//
//  `enum class` (scoped) — `ServerStatus::UP` likhna padta, int me chupke se
//  convert nahi hota (type safety).
// ============================================================================
#ifndef LOADBALANCER_LLD_ENUMS_SERVERSTATUS_H
#define LOADBALANCER_LLD_ENUMS_SERVERSTATUS_H

namespace load_balancer_lld {

enum class ServerStatus { UP, DOWN };

} // namespace load_balancer_lld

#endif // LOADBALANCER_LLD_ENUMS_SERVERSTATUS_H
