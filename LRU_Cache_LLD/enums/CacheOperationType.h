// ============================================================================
//  enums/CacheOperationType.h  —  Cache pe hone wale operations ke naam
// ----------------------------------------------------------------------------
//  5 operations jo cache support karta hai. Ye enum mukhya taur pe DEMO/LOGGING
//  ke liye hai — CacheService ka executeAndDescribe() is enum ke hisaab se
//  operation chala ke ek readable line banata hai ("GET key=user:1 => HIT ...").
//
//  📌 Enum + toString ka fayda: demo me operation ko "data" ki tarah loop me
//     paas kar sakte hain aur seedha print bhi. Ek generic "operation runner" banta.
//
//  `inline` isliye taaki ye header kai .cpp me include ho to linker "duplicate
//  symbol" ka error na de.
// ============================================================================
#ifndef LRU_CACHE_LLD_ENUMS_CACHEOPERATIONTYPE_H
#define LRU_CACHE_LLD_ENUMS_CACHEOPERATIONTYPE_H

#include <string>

namespace lru_cache_lld {

enum class CacheOperationType {
    GET,      // value dhoondho (recency badalta hai)
    PUT,      // insert/update (MRU banta hai)
    CONTAINS, // sirf check (recency nahi badalta)
    REMOVE,   // key hatao
    CLEAR     // sab khaali
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
