#ifndef LRU_CACHE_LLD_CONFIG_CACHECONFIG_H
#define LRU_CACHE_LLD_CONFIG_CACHECONFIG_H

#include <stdexcept>
#include <string>

namespace lru_cache_lld {

class CacheConfig {
public:
    explicit CacheConfig(size_t capacity) : capacity_(capacity) {
        if (capacity_ == 0) {
            throw std::invalid_argument("Cache capacity must be greater than 0");
        }
    }

    size_t getCapacity() const { return capacity_; }

    void setCapacity(size_t capacity) {
        if (capacity == 0) {
            throw std::invalid_argument("Cache capacity must be greater than 0");
        }
        capacity_ = capacity;
    }

private:
    size_t capacity_;
};

} // namespace lru_cache_lld

#endif // LRU_CACHE_LLD_CONFIG_CACHECONFIG_H
