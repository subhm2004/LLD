#ifndef LRU_CACHE_LLD_CACHE_ICACHE_H
#define LRU_CACHE_LLD_CACHE_ICACHE_H

#include <optional>
#include <cstddef>

namespace lru_cache_lld {

template <typename Key, typename Value>
class ICache {
public:
    virtual ~ICache() = default;

    virtual std::optional<Value> get(const Key &key) = 0;
    virtual void put(const Key &key, const Value &value) = 0;
    virtual bool contains(const Key &key) const = 0;
    virtual bool remove(const Key &key) = 0;
    virtual void clear() = 0;
    virtual size_t size() const = 0;
    virtual size_t capacity() const = 0;
};

} // namespace lru_cache_lld

#endif // LRU_CACHE_LLD_CACHE_ICACHE_H
