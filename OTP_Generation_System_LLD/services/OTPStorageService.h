#ifndef OTP_GENERATION_SYSTEM_LLD_SERVICES_OTPSTORAGESERVICE_H
#define OTP_GENERATION_SYSTEM_LLD_SERVICES_OTPSTORAGESERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../models/OTPRecord.h"

namespace otp_lld {

class OTPStorageService {
public:
    void save(const OTPRecord &record) { recordsById_[record.otpId] = record; }

    OTPRecord *findById(const std::string &otpId) {
        return const_cast<OTPRecord *>(findByIdConst(otpId));
    }

    const OTPRecord *findByIdConst(const std::string &otpId) const {
        auto it = recordsById_.find(otpId);
        if (it == recordsById_.end()) {
            return nullptr;
        }
        return &it->second;
    }

    void update(const OTPRecord &record) { recordsById_[record.otpId] = record; }

    void invalidatePendingForUser(const std::string &userId, OTPPurpose purpose) {
        for (auto &entry : recordsById_) {
            if (entry.second.userId == userId && entry.second.purpose == purpose &&
                entry.second.status == OTPStatus::PENDING) {
                entry.second.status = OTPStatus::EXPIRED;
            }
        }
    }

private:
    std::unordered_map<std::string, OTPRecord> recordsById_;
};

} // namespace otp_lld

#endif // OTP_GENERATION_SYSTEM_LLD_SERVICES_OTPSTORAGESERVICE_H
