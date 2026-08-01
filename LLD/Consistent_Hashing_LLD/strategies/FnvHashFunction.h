// ============================================================================
//  strategies/FnvHashFunction.h  —  CONCRETE STRATEGY: FNV-1a (32-bit)
// ----------------------------------------------------------------------------
//  FNV-1a duniya ka sabse simple achha hash hai — ek loop, ek XOR, ek multiply.
//  Padhne me itna aasan ki interview me board pe likh sakte ho.
//
//      hash = 2166136261                  (FNV offset basis)
//      har byte ke liye:
//          hash = hash XOR byte           <- pehle XOR (isi liye "1a" variant)
//          hash = hash × 16777619         (FNV prime)
//
//  ⭐ "1a" variant hi kyun (FNV-1 nahi)? FNV-1 me multiply pehle hota hai aur
//     XOR baad me. FNV-1a ulta karta hai, aur usse avalanche behtar milta hai.
//     Practically FNV-1a hi use hota hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠ FNV-1a AKELA consistent hashing ke liye THODA KAMZOR hai              │
//  │                                                                          │
//  │  Milti-julti chhoti strings pe ("cache-1#0" vs "cache-1#1") uske LOWER    │
//  │  bits kam bikharte hain. Aur virtual node names to bilkul aise hi hote    │
//  │  hain — ek hi prefix, sirf aakhri number badalta hai! Agar wo ring pe     │
//  │  paas-paas gir gaye to vnodes ka poora maqsad hi mar jaayega.             │
//  │                                                                          │
//  │  ✅ Ilaaj: aakhir me `fmix32` (murmur3 ka finalizer) laga do. Wo koi nayi │
//  │  information add nahi karta — bas maujooda bits ko achhe se "hila" deta   │
//  │  hai. Chhota sa step, par distribution me bada farak.                    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Kab use karein: fast, simple, aur non-crypto. Yahi default rakha hai.
//     Crypto security ki zaroorat NAHI hai yahan — sirf achha DISTRIBUTION
//     chahiye. Isi liye MD5/SHA (slow) ki jagah ye theek hai.
// ============================================================================
#ifndef CONSISTENT_HASHING_LLD_STRATEGIES_FNVHASHFUNCTION_H
#define CONSISTENT_HASHING_LLD_STRATEGIES_FNVHASHFUNCTION_H

#include <cstdint>
#include <string>

#include "HashFunction.h"

using namespace std;

namespace consistent_hashing_lld {

class FnvHashFunction : public HashFunction {
public:
    uint32_t hash(const string &key) const override {
        uint32_t h = FNV_OFFSET_BASIS;
        for (unsigned char c : key) {
            h ^= c;          // pehle XOR
            h *= FNV_PRIME;  // phir multiply
        }
        return finalMix(h); // ⭐ avalanche — upar wala note dekho
    }

    string name() const override { return "FNV-1a-32"; }

private:
    static const uint32_t FNV_OFFSET_BASIS = 2166136261u;
    static const uint32_t FNV_PRIME = 16777619u;

    // murmur3 ka final mix — bits ko phailata hai, nayi info add nahi karta.
    static uint32_t finalMix(uint32_t h) {
        h ^= h >> 16;
        h *= 0x85ebca6bu;
        h ^= h >> 13;
        h *= 0xc2b2ae35u;
        h ^= h >> 16;
        return h;
    }
};

} // namespace consistent_hashing_lld

#endif // CONSISTENT_HASHING_LLD_STRATEGIES_FNVHASHFUNCTION_H
