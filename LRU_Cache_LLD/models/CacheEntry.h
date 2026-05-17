#ifndef LRU_CACHE_LLD_MODELS_CACHEENTRY_H
#define LRU_CACHE_LLD_MODELS_CACHEENTRY_H

#include <string>
#include <utility>

namespace lru_cache_lld {

template <typename Key, typename Value>
struct CacheEntry {
    Key key;
    Value value;

    CacheEntry(Key keyValue, Value valueData)
        : key(std::move(keyValue)), value(std::move(valueData)) {}
};

} // namespace lru_cache_lld

#endif // LRU_CACHE_LLD_MODELS_CACHEENTRY_H
