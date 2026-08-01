// ============================================================================
//  enums/EvictionPolicyType.h  —  Kaunsi eviction policy chal rahi hai
// ----------------------------------------------------------------------------
//  Abhi is folder me sirf LFU hai, to enum me ek hi value: LFU. Ye label mostly
//  printing/reporting ke liye hai (CacheService::printState "Policy: LFU" dikhata).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ek hi value ka enum "over-engineering" lagta hai — par future ke liye │
//  │     seed hai                                                              │
//  │                                                                          │
//  │  Aage LRU / FIFO / RANDOM add karne ho, to yahan entry aayegi aur ek      │
//  │  Factory in policies me se cache bana ke de sakti hai (ICache* return).   │
//  │  Abhi ek hi hai kyunki ye folder specifically LFU ke liye hai — sibling   │
//  │  folder LRU_Cache_LLD me LRU wali dusri implementation hai.               │
//  │                                                                          │
//  │  📌 "Abhi kitna chahiye" ke hisaab se rakha (ek value); "kabhi kya ho     │
//  │     sakta hai" ke liye enum + toString ka dhaancha ready hai.            │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef LFU_CACHE_LLD_ENUMS_EVICTIONPOLICYTYPE_H
#define LFU_CACHE_LLD_ENUMS_EVICTIONPOLICYTYPE_H

#include <string>

namespace lfu_cache_lld {

enum class EvictionPolicyType {
    LFU // Least Frequently Used (aage LRU/FIFO yahan add ho sakte hain)
};

inline std::string evictionPolicyTypeToString(const EvictionPolicyType policy) {
    switch (policy) {
    case EvictionPolicyType::LFU:
        return "LFU";
    default:
        return "UNKNOWN";
    }
}

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_ENUMS_EVICTIONPOLICYTYPE_H
