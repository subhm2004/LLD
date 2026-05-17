#ifndef LFU_CACHE_LLD_CACHE_ICACHE_H
#define LFU_CACHE_LLD_CACHE_ICACHE_H

#include <cstddef>
#include <optional>

#include "../config/CppStandard.h"

namespace lfu_cache_lld {

template <typename Key, typename Value>
class ICache {
public:
    virtual ~ICache() = default;

    [[nodiscard]] virtual std::optional<Value> get(const Key &key) = 0;
    virtual void put(const Key &key, const Value &value) = 0;
    [[nodiscard]] virtual bool contains(const Key &key) const = 0;
    [[nodiscard]] virtual bool remove(const Key &key) = 0;
    virtual void clear() = 0;
    [[nodiscard]] virtual std::size_t size() const = 0;
    [[nodiscard]] virtual std::size_t capacity() const = 0;
};

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_CACHE_ICACHE_H
