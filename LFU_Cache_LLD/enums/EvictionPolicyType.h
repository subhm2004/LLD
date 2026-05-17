#ifndef LFU_CACHE_LLD_ENUMS_EVICTIONPOLICYTYPE_H
#define LFU_CACHE_LLD_ENUMS_EVICTIONPOLICYTYPE_H

#include <string>

#include <bits/stdc++.h>

namespace lfu_cache_lld {

enum class EvictionPolicyType {
    LFU
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
