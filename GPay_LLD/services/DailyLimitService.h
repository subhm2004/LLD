#ifndef GPAY_LLD_SERVICES_DAILYLIMITSERVICE_H
#define GPAY_LLD_SERVICES_DAILYLIMITSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>

namespace gpay_lld {

class DailyLimitService {
public:
    explicit DailyLimitService(double dailyLimitInr = 100000.0) : dailyLimit_(dailyLimitInr) {}

    void recordSpend(const std::string& userId, double amount) {
        const double next = spentToday_[userId] + amount;
        if (next > dailyLimit_) {
            throw std::runtime_error("daily UPI limit exceeded");
        }
        spentToday_[userId] = next;
    }

    double getRemaining(const std::string& userId) const {
        auto it = spentToday_.find(userId);
        if (it == spentToday_.end()) {
            return dailyLimit_;
        }
        return dailyLimit_ - it->second;
    }

private:
    double dailyLimit_;
    std::unordered_map<std::string, double> spentToday_;
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_SERVICES_DAILYLIMITSERVICE_H
