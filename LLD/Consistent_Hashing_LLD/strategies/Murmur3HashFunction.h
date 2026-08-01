// ============================================================================
//  strategies/Murmur3HashFunction.h  —  CONCRETE STRATEGY: MurmurHash3 (32-bit)
// ----------------------------------------------------------------------------
//  Ye wahi algorithm hai jo **Cassandra** apne Murmur3Partitioner me use karta
//  hai (wahan 128-bit variant hai, yahan 32-bit — idea bilkul same).
//
//  ⭐ FNV se behtar KYUN? FNV byte-by-byte chalta hai. Murmur3 ek saath 4 BYTES
//     (ek 32-bit block) uthata hai aur har block ko rotate + multiply se achhe
//     se mix karta hai. Isliye:
//        * tez hai (kam iterations, aur CPU ko pipeline karne me aasan)
//        * avalanche behtar hai (milti-julti strings bhi door-door girti hain)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ ALGORITHM 3 HISSON ME                                                 │
//  │                                                                          │
//  │  1. BODY  — string ko 4-4 byte ke blocks me todo. Har block ko do magic   │
//  │     constants (c1, c2) se multiply aur beech me rotate karo, phir running │
//  │     hash me mila do.                                                     │
//  │                                                                          │
//  │  2. TAIL  — jo 1-3 bytes bache (length 4 ka multiple nahi thi), unko      │
//  │     alag se mila do. Isi liye neeche `switch` me fallthrough hai —        │
//  │     3 bytes bache to teeno chahiye, 2 bache to do. Ye jaan-bujh ke hai,   │
//  │     bug nahi (`[[fallthrough]]` se compiler ko bata diya).                │
//  │                                                                          │
//  │  3. FINALIZE — length mila ke `fmix32` chalao. Ye final avalanche step    │
//  │     hai jo baaki saare bits ko theek se hila deta hai.                    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `seed` kyun? Same key ka alag-alag hash chahiye ho (jaise alag-alag ring
//     ya bloom filter ke alag hash functions) to seed badal do. Yahan default
//     0 rakha hai taaki output har jagah reproducible rahe.
//
//  ⚠ `memcpy` se block padhte hain, `*(uint32_t*)ptr` cast se nahi — kyunki wo
//     cast unaligned read / strict-aliasing UB de sakta hai. Compiler `memcpy`
//     ko waise bhi ek single load me optimize kar deta hai, to speed ka nuksan
//     nahi hota. (Chhoti si baat, par yahi "production-grade" banati hai.)
// ============================================================================
#ifndef CONSISTENT_HASHING_LLD_STRATEGIES_MURMUR3HASHFUNCTION_H
#define CONSISTENT_HASHING_LLD_STRATEGIES_MURMUR3HASHFUNCTION_H

#include <cstdint>
#include <cstring>
#include <string>

#include "HashFunction.h"

using namespace std;

namespace consistent_hashing_lld {

class Murmur3HashFunction : public HashFunction {
public:
    explicit Murmur3HashFunction(uint32_t seed = 0) : seed_(seed) {}

    uint32_t hash(const string &key) const override {
        const uint8_t *data = reinterpret_cast<const uint8_t *>(key.data());
        const size_t length = key.size();
        const size_t blockCount = length / 4;

        uint32_t h = seed_;

        // ---- 1. BODY — 4-4 byte ke blocks ---------------------------------
        for (size_t i = 0; i < blockCount; ++i) {
            uint32_t k;
            memcpy(&k, data + i * 4, sizeof(k)); // safe unaligned read

            k *= C1;
            k = rotateLeft(k, 15);
            k *= C2;

            h ^= k;
            h = rotateLeft(h, 13);
            h = h * 5 + 0xe6546b64u;
        }

        // ---- 2. TAIL — bache hue 1-3 bytes --------------------------------
        // Fallthrough JAAN-BUJH KE hai: 3 bache to teeno bytes chahiye.
        const uint8_t *tail = data + blockCount * 4;
        uint32_t k1 = 0;
        switch (length & 3) {
        case 3:
            k1 ^= static_cast<uint32_t>(tail[2]) << 16;
            [[fallthrough]];
        case 2:
            k1 ^= static_cast<uint32_t>(tail[1]) << 8;
            [[fallthrough]];
        case 1:
            k1 ^= static_cast<uint32_t>(tail[0]);
            k1 *= C1;
            k1 = rotateLeft(k1, 15);
            k1 *= C2;
            h ^= k1;
            break;
        default:
            break; // length 4 ka poora multiple thi — kuch bacha hi nahi
        }

        // ---- 3. FINALIZE ---------------------------------------------------
        h ^= static_cast<uint32_t>(length);
        return finalMix(h);
    }

    string name() const override { return "Murmur3-32"; }

private:
    // Murmur3 ke magic constants (paper/reference implementation se).
    static const uint32_t C1 = 0xcc9e2d51u;
    static const uint32_t C2 = 0x1b873593u;

    // Bits ko circular shift — jo left se bahar nikle wo right se ghus jaaye.
    static uint32_t rotateLeft(uint32_t x, int8_t r) {
        return (x << r) | (x >> (32 - r));
    }

    static uint32_t finalMix(uint32_t h) {
        h ^= h >> 16;
        h *= 0x85ebca6bu;
        h ^= h >> 13;
        h *= 0xc2b2ae35u;
        h ^= h >> 16;
        return h;
    }

    uint32_t seed_;
};

} // namespace consistent_hashing_lld

#endif // CONSISTENT_HASHING_LLD_STRATEGIES_MURMUR3HASHFUNCTION_H
