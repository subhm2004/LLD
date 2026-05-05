#ifndef SLIDING_WINDOW_LOG_RATE_LIMITER_H
#define SLIDING_WINDOW_LOG_RATE_LIMITER_H

#include <string>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <chrono>
#include "RateLimiter.h"
#include "../enums/RateLimitType.h"
#include "../model/RateLimitConfig.h"

using namespace std;
namespace rate_limiter_lld {
class SlidingWindowLogRateLimiter : public RateLimiter {
private:
    unordered_map<string, queue<long long>> requestLog;
    mutex mtx;

public:
    SlidingWindowLogRateLimiter(RateLimitConfig config)
        : RateLimiter(config, RateLimitType::SLIDING_WINDOW_LOG) {}

    bool allowRequest(const string& userId) override {
        lock_guard<mutex> lock(mtx);

        long long now = chrono::duration_cast<chrono::seconds>(
            chrono::system_clock::now().time_since_epoch()).count();

        auto& log = requestLog[userId];

        // Remove timestamps outside the window
        while (!log.empty() && (now - log.front()) >= config.getWindowInSeconds()) {
            log.pop();
        }

        if ((int)log.size() < config.getMaxRequests()) {
            log.push(now);
            return true;
        }

        return false;
    }
};
}

#endif // SLIDING_WINDOW_LOG_RATE_LIMITER_H