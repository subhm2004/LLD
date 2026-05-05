#ifndef FIXED_WINDOW_RATE_LIMITER_H
#define FIXED_WINDOW_RATE_LIMITER_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include "RateLimiter.h"
#include "../enums/RateLimitType.h"
#include "../model/RateLimitConfig.h"

using namespace std;
namespace rate_limiter_lld {
class FixedWindowRateLimiter : public RateLimiter
{
private:
    unordered_map<string, int> requestCount;
    unordered_map<string, long long> windowStart;
    mutex mtx;

public:
    FixedWindowRateLimiter(RateLimitConfig config)
        : RateLimiter(config, RateLimitType::FIXED_WINDOW) {}

    bool allowRequest(const string &userId) override
    {
        lock_guard<mutex> lock(mtx);

        long long nowMs = chrono::duration_cast<chrono::seconds>(
                              chrono::system_clock::now().time_since_epoch())
                              .count();

        long long currentReqWindow = nowMs / config.getWindowInSeconds();

        long long lastReqWindow = windowStart.count(userId)
                                      ? windowStart[userId]
                                      : currentReqWindow;

        // Window expired -> reset
        if (lastReqWindow != currentReqWindow)
        {
            windowStart[userId] = currentReqWindow;
            requestCount[userId] = 1;
            return true;
        }

        int count = requestCount.count(userId) ? requestCount[userId] : 0;

        if (count < config.getMaxRequests())
        {
            requestCount[userId] = count + 1;
            return true;
        }

        return false;
    }
};
}

#endif // FIXED_WINDOW_RATE_LIMITER_H