#ifndef TOKEN_BUCKET_RATE_LIMITER_H
#define TOKEN_BUCKET_RATE_LIMITER_H

#include <string>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <algorithm>
#include "RateLimiter.h"
#include "../enums/RateLimitType.h"
#include "../model/RateLimitConfig.h"

using namespace std;
namespace rate_limiter_lld {
class TokenBucketRateLimiter : public RateLimiter
{
private:
    unordered_map<string, int> tokens;
    unordered_map<string, long long> lastRefillTime;
    mutex mtx;

    int refillTokens(const string &userId, long long now)
    {
        double refillRate = (double)config.getWindowInSeconds() / config.getMaxRequests();

        long long lastRefill = lastRefillTime.count(userId)
                                   ? lastRefillTime[userId]
                                   : now;

        long long elapsedSeconds = (now - lastRefill) / 1000;
        int refillAmt = (int)(elapsedSeconds / refillRate);

        int currentTokens = tokens.count(userId)
                                ? tokens[userId]
                                : config.getMaxRequests();

        currentTokens = min(config.getMaxRequests(), currentTokens + refillAmt);

        if (refillAmt > 0)
            lastRefillTime[userId] = now;

        return currentTokens;
    }

public:
    TokenBucketRateLimiter(RateLimitConfig config)
        : RateLimiter(config, RateLimitType::TOKEN_BUCKET) {}

    bool allowRequest(const string &userId) override
    {
        lock_guard<mutex> lock(mtx);

        long long now = chrono::duration_cast<chrono::milliseconds>(
                            chrono::system_clock::now().time_since_epoch())
                            .count();

        int currentTokens = refillTokens(userId, now);

        if (currentTokens > 0)
        {
            tokens[userId] = currentTokens - 1;
            return true;
        }

        tokens[userId] = 0;
        return false;
    }
};
}

#endif // TOKEN_BUCKET_RATE_LIMITER_H