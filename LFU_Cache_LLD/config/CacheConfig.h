#ifndef LFU_CACHE_LLD_CONFIG_CACHECONFIG_H
#define LFU_CACHE_LLD_CONFIG_CACHECONFIG_H

#include <cstddef>
#include <stdexcept>

#include "CppStandard.h"

namespace lfu_cache_lld {

class CacheConfig {
public:
    explicit CacheConfig(std::size_t capacity) : capacity_(capacity) {
        if (capacity_ == 0U) {
            throw std::invalid_argument("Cache capacity must be greater than 0");
        }
    }

    std::size_t getCapacity() const noexcept { return capacity_; }

private:
    std::size_t capacity_;
};

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_CONFIG_CACHECONFIG_H
