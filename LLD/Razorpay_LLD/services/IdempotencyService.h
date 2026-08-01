#ifndef RAZORPAY_LLD_SERVICES_IDEMPOTENCY_SERVICE_H
#define RAZORPAY_LLD_SERVICES_IDEMPOTENCY_SERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>

namespace razorpay_lld {

class IdempotencyService {
public:
    std::string getOrThrow(const std::string& key) const {
        auto it = keyToResourceId_.find(key);
        if (it == keyToResourceId_.end()) {
            throw std::runtime_error("idempotency key not found: " + key);
        }
        return it->second;
    }

    bool exists(const std::string& key) const { return keyToResourceId_.count(key) > 0; }

    void store(const std::string& key, const std::string& resourceId) {
        if (exists(key)) {
            throw std::runtime_error("idempotency key already used: " + key);
        }
        keyToResourceId_[key] = resourceId;
    }

    std::string resolveDuplicate(const std::string& key) const {
        auto it = keyToResourceId_.find(key);
        return it == keyToResourceId_.end() ? "" : it->second;
    }

private:
    std::unordered_map<std::string, std::string> keyToResourceId_;
};

}  // namespace razorpay_lld

#endif
