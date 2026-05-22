#ifndef THREAD_SAFE_TTL_CACHE_LLD_MODELS_CACHEENTRY_H
#define THREAD_SAFE_TTL_CACHE_LLD_MODELS_CACHEENTRY_H

#include <chrono>
#include <string>

namespace ttl_cache_lld {

struct CacheEntry {
    std::string value;
    std::chrono::steady_clock::time_point expires_at;

    bool isExpired() const {
        return std::chrono::steady_clock::now() >= expires_at;
    }

    long long remainingMs() const {
        auto now = std::chrono::steady_clock::now();
        if (now >= expires_at) {
            return 0;
        }
        return std::chrono::duration_cast<std::chrono::milliseconds>(expires_at - now)
            .count();
    }
};

}  // namespace ttl_cache_lld

#endif  // THREAD_SAFE_TTL_CACHE_LLD_MODELS_CACHEENTRY_H
