// ============================================================================
//  strategies/HashFunction.h  —  STRATEGY PATTERN ka interface
// ----------------------------------------------------------------------------
//  "String ko ring position (0 .. 2^32-1) me kaise badlein?" — ye ek BADALNE
//  WALA faisla hai. MD5? Murmur3? FNV? xxHash? Har algorithm alag trade-off
//  deta hai (speed vs distribution quality). Isi liye is faisle ko ek INTERFACE
//  ke peeche rakh diya — yahi Strategy Pattern.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ STRATEGY PATTERN — "algorithm ko plug-and-play banao"                 │
//  │                                                                          │
//  │  ConsistentHashRing ek `HashFunction*` rakhta hai — usse pata nahi andar  │
//  │  kaunsa algorithm hai. Wo bas `hash(key)` bulata hai. Kaunsi ring         │
//  │  position banegi, wo inject ki gayi concrete strategy decide karti hai.   │
//  │                                                                          │
//  │  Real duniya me sach me alag-alag chalte hain:                            │
//  │     Memcached (ketama) -> MD5    |  Cassandra -> Murmur3                  │
//  │     Riak               -> SHA-1  |  bahut se internal systems -> xxHash   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠ YAHAN STRATEGY SWAP MUFT NAHI HAI (LoadBalancer_LLD se bada farak)    │
//  │                                                                          │
//  │  Load balancer me strategy badlo -> agli request pe naya algorithm, bas.  │
//  │  Par yahan hash function badalne ka matlab hai: har node ki ring position │
//  │  BADAL GAYI -> poora ring dobara banana padega -> LAGBHAG SAARI keys      │
//  │  move hongi.                                                             │
//  │                                                                          │
//  │  Isi liye `ConsistentHashRing::setHashFunction()` ring ko REBUILD karta   │
//  │  hai aur listeners ko warn karta hai. Production me ye ek planned         │
//  │  migration hoti hai, "runtime toggle" nahi.                              │
//  │                                                                          │
//  │  ⭐ Interview point: "Strategy pattern lagana aasan hai, par uske swap    │
//  │     ki KEEMAT domain pe depend karti hai" — yahi wo keemat hai.           │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ HASH FUNCTION SE KYA CHAHIYE (dono zaroori):
//     1. DETERMINISTIC — same input -> hamesha same output, har machine pe,
//        har run me. Warna ek server key ko "A" pe dhundhega, doosra "B" pe.
//     2. UNIFORM (avalanche) — input ka 1 bit badle to output ke ~aadhe bits
//        badlein. Isse "cache-1#0", "cache-1#1" ring pe door-door bikhrenge,
//        ek jagah cluster nahi honge (warna vnodes ka poora faayda khatam).
//
//  ⚠ `std::hash<string>` JAAN-BUJH KE nahi use kiya — standard guarantee nahi
//     deta ki value process/library ke beech same rahegi. Distributed system me
//     har node ko BILKUL same number chahiye.
//
//  ⭐ VIRTUAL DESTRUCTOR zaroori: ring `unique_ptr<HashFunction>` me derived
//     object base pointer ke through rakhta hai. Bina virtual destructor ke
//     derived ka cleanup chalta hi nahi -> undefined behavior. `= default`
//     isliye ki interface ke paas khud delete karne ko kuch nahi.
// ============================================================================
#ifndef CONSISTENT_HASHING_LLD_STRATEGIES_HASHFUNCTION_H
#define CONSISTENT_HASHING_LLD_STRATEGIES_HASHFUNCTION_H

#include <cstdint>
#include <string>

using namespace std;

namespace consistent_hashing_lld {

class HashFunction {
public:
    virtual ~HashFunction() = default;

    // Pure virtual (`= 0`) — har concrete strategy ko implement karna HI padega.
    // Return: ring pe position (0 se 2^32-1).
    virtual uint32_t hash(const string &key) const = 0;

    // Stats/logging me dikhane ke liye ("abhi kaunsa hash chal raha hai").
    virtual string name() const = 0;
};

} // namespace consistent_hashing_lld

#endif // CONSISTENT_HASHING_LLD_STRATEGIES_HASHFUNCTION_H
