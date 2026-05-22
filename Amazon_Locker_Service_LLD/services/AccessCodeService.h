#ifndef AMAZON_LOCKER_SERVICE_LLD_SERVICES_ACCESSCODESERVICE_H
#define AMAZON_LOCKER_SERVICE_LLD_SERVICES_ACCESSCODESERVICE_H

#include <chrono>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../enums/AccessCodeStatus.h"
#include "../models/AccessCode.h"

namespace amazon_locker_lld {

class AccessCodeService {
public:
    explicit AccessCodeService(int codeLength = 6, int validityHours = 72, int maxAttempts = 3)
        : codeLength_(codeLength),
          validityHours_(validityHours),
          maxAttempts_(maxAttempts),
          codeCounter_(0) {}

    AccessCode generate(const std::string& packageId, const std::string& compartmentId, long long nowEpoch) {
        const std::string code = generateNumericCode();
        const std::string codeId = "AC_" + std::to_string(++codeCounter_);
        const long long expiresAt = nowEpoch + static_cast<long long>(validityHours_) * 3600LL;

        AccessCode accessCode(codeId, code, packageId, compartmentId, nowEpoch, expiresAt, maxAttempts_);
        codesById_.insert_or_assign(codeId, accessCode);
        codesByValue_.insert_or_assign(code, codeId);
        return accessCode;
    }

    AccessCode& getByCode(const std::string& enteredCode) {
        auto it = codesByValue_.find(enteredCode);
        if (it == codesByValue_.end()) {
            throw std::invalid_argument("Invalid pickup code");
        }
        return codesById_.at(it->second);
    }

    const AccessCode& getByCode(const std::string& enteredCode) const {
        auto it = codesByValue_.find(enteredCode);
        if (it == codesByValue_.end()) {
            throw std::invalid_argument("Invalid pickup code");
        }
        return codesById_.at(it->second);
    }

    void save(AccessCode& code) {
        codesById_.insert_or_assign(code.codeId(), code);
        packageToCode_.insert_or_assign(code.packageId(), code.code());
    }

    std::string getCodeForPackage(const std::string& packageId) const {
        auto it = packageToCode_.find(packageId);
        if (it == packageToCode_.end()) {
            throw std::invalid_argument("No access code for package: " + packageId);
        }
        return it->second;
    }

    int validityHours() const { return validityHours_; }

private:
    std::string generateNumericCode() {
        std::ostringstream oss;
        std::uniform_int_distribution<int> dist(0, 9);
        std::mt19937 rng(static_cast<unsigned>(
            std::chrono::steady_clock::now().time_since_epoch().count()));
        for (int i = 0; i < codeLength_; ++i) {
            oss << dist(rng);
        }
        return oss.str();
    }

    int codeLength_;
    int validityHours_;
    int maxAttempts_;
    int codeCounter_;
    std::unordered_map<std::string, AccessCode> codesById_;
    std::unordered_map<std::string, std::string> codesByValue_;
    std::unordered_map<std::string, std::string> packageToCode_;
};

}  // namespace amazon_locker_lld

#endif  // AMAZON_LOCKER_SERVICE_LLD_SERVICES_ACCESSCODESERVICE_H
