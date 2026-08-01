// ============================================================================
//  enums/NodeStatus.h  —  Node abhi keys le sakta hai ya nahi
// ----------------------------------------------------------------------------
//     ACTIVE -> node healthy hai, keys ispe route ho sakti hain
//     DOWN   -> node unhealthy hai, ispe keys mat bhejo (par ring pe RAHEGA)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DOWN aur REMOVE me FARAK — ye consistent hashing ka bada point hai    │
//  │                                                                          │
//  │  DOWN   = node ring pe hai, bas abhi keys nahi le raha (temporary).       │
//  │           Uski keys clockwise agle ACTIVE node pe chali jaati hain.       │
//  │           Wapas ACTIVE karte hi uski keys usko WAPAS mil jaati hain —     │
//  │           kyunki ring ki positions hili hi nahi thi.                      │
//  │                                                                          │
//  │  REMOVE = node ke saare virtual nodes ring se HAT gaye (permanent).       │
//  │           Ab wo wapas add hua to positions phir se compute hongi.         │
//  │                                                                          │
//  │  Isi liye chhoti si network glitch pe node ko REMOVE nahi karte —         │
//  │  bas DOWN mark karte hain. Warna har blip pe poora data migration         │
//  │  shuru ho jaayega (mahenga aur bekaar).                                   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Real system me ye status gossip protocol / health probes se automatically
//     set hota hai (Cassandra me gossip, Dynamo me failure detector). Yahan
//     demo hai isliye `markNodeStatus()` se manually simulate karte hain
//     (requirements ki Assumption me likha hai).
//
//  `enum class` (scoped) — `NodeStatus::ACTIVE` likhna padta, int me chupke se
//  convert nahi hota (type safety).
// ============================================================================
#ifndef CONSISTENT_HASHING_LLD_ENUMS_NODESTATUS_H
#define CONSISTENT_HASHING_LLD_ENUMS_NODESTATUS_H

namespace consistent_hashing_lld {

enum class NodeStatus { ACTIVE, DOWN };

} // namespace consistent_hashing_lld

#endif // CONSISTENT_HASHING_LLD_ENUMS_NODESTATUS_H
