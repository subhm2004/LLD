// ============================================================================
//  enums/CacheOperationType.h  —  Cache pe hone wale operations ke naam
// ----------------------------------------------------------------------------
//  5 operations jo cache support karta hai. Ye enum mukhya taur pe DEMO/LOGGING
//  ke liye hai — CacheService ka executeAndDescribe() is enum ke hisaab se
//  operation chala ke ek readable line banata hai ("GET key=A => HIT ...").
//
//  📌 Enum + toString helper ka fayda: demo code me operation ko "data" ki tarah
//     paas kar sakte hain (loop me alag-alag operations), aur unhe seedha print
//     bhi kar sakte hain. Ek generic "operation runner" ban jaata hai.
//
//  `inline` function isliye taaki ye header kai .cpp me include ho to linker
//  "duplicate symbol" ka error na de.
// ============================================================================
#ifndef LFU_CACHE_LLD_ENUMS_CACHEOPERATIONTYPE_H
#define LFU_CACHE_LLD_ENUMS_CACHEOPERATIONTYPE_H

#include <string>

namespace lfu_cache_lld {

enum class CacheOperationType {
    GET,      // value dhoondho (freq badhta hai)
    PUT,      // insert/update (freq badhta hai)
    CONTAINS, // sirf check (freq nahi badalta)
    REMOVE,   // key hatao
    CLEAR     // sab khaali
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
