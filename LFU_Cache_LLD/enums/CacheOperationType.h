#ifndef LFU_CACHE_LLD_ENUMS_CACHEOPERATIONTYPE_H
#define LFU_CACHE_LLD_ENUMS_CACHEOPERATIONTYPE_H

#include <string>

#include <bits/stdc++.h>

namespace lfu_cache_lld {

enum class CacheOperationType {
    GET,
    PUT,
    CONTAINS,
    REMOVE,
    CLEAR
};

inline std::string cacheOperationTypeToString(const CacheOperationType type) {
    switch (type) {
    case CacheOperationType::GET:
        return "GET";
    case CacheOperationType::PUT:
        return "PUT";
    case CacheOperationType::CONTAINS:
        return "CONTAINS";
    case CacheOperationType::REMOVE:
        return "REMOVE";
    case CacheOperationType::CLEAR:
        return "CLEAR";
    default:
        return "UNKNOWN";
    }
}

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_ENUMS_CACHEOPERATIONTYPE_H
