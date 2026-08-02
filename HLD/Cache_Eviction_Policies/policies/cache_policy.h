// ============================================================================
//  policies/cache_policy.h  —  Saari eviction policies ka COMMON interface
// ----------------------------------------------------------------------------
//  Cache ka asli sawaal ek hi hai: "jagah khatam ho gayi — ab KISKO nikalein?"
//
//  Har policy ka jawab alag hai:
//     FIFO   -> "jo sabse pehle aaya tha"
//     LRU    -> "jise sabse der se use nahi kiya"
//     LFU    -> "jise sabse KAM baar use kiya"
//     CLOCK  -> "LRU ka sasta andaaza"
//     Random -> "koi bhi, jo mile"
//     ARC    -> "haalat dekh kar khud decide karunga"
//
//  ⭐ Ye faisla ek BADALNE WALA algorithm hai — isliye interface ke peeche
//     rakh diya (Strategy pattern, wahi jo LoadBalancer_LLD me tha).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ HUM SIRF HIT RATE NAAPTE HAIN — value store nahi karte               │
//  │                                                                          │
//  │  In demos me cache asli data rakhta hi nahi. Kyunki sawaal ye hai ki      │
//  │  "kaunsi policy zyada HIT deti hai", "data kaise store hota hai" nahi.    │
//  │  Value rakhne se code bada hota aur seekhne wali baat chhup jaati.        │
//  │                                                                          │
//  │  Asli implementation (map + list ke saath value) tumhare LLD folders me   │
//  │  hai: LRU_Cache_LLD aur LFU_Cache_LLD.                                    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ VIRTUAL DESTRUCTOR zaroori — policies base pointer (`unique_ptr`) ke
//     through rakhi jaati hain. Bina iske derived ka cleanup chalega hi nahi.
// ============================================================================
#ifndef CACHE_EVICTION_POLICIES_CACHE_POLICY_H
#define CACHE_EVICTION_POLICIES_CACHE_POLICY_H

#include <cstddef>
#include <string>

using namespace std;

namespace cache
{

class CachePolicy
{
public:
    virtual ~CachePolicy() = default;

    // Stable naam, jaise "LRU"
    virtual const char *name() const = 0;

    // ⭐ Ek access. Return: true = HIT, false = MISS (aur andar daal diya gaya)
    virtual bool access(const string &key) = 0;

    // Cache khaali karo (agla workload chalane se pehle)
    virtual void clear() = 0;

    virtual size_t capacity() const = 0;
};

} // namespace cache

#endif // CACHE_EVICTION_POLICIES_CACHE_POLICY_H
