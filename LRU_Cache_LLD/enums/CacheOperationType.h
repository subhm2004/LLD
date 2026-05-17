#ifndef LRU_CACHE_LLD_ENUMS_CACHEOPERATIONTYPE_H
#define LRU_CACHE_LLD_ENUMS_CACHEOPERATIONTYPE_H

#include <string>

namespace lru_cache_lld {

enum class CacheOperationType {
    GET,
    PUT,
    CONTAINS,
    REMOVE,
    CLEAR
};

inline std::string cacheOperationTypeToString(CacheOperationType type) {
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

} // namespace lru_cache_lld

#endif // LRU_CACHE_LLD_ENUMS_CACHEOPERATIONTYPE_H
