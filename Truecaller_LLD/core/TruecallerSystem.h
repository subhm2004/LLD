#ifndef TRUECALLER_LLD_CORE_TRUECALLERSYSTEM_H
#define TRUECALLER_LLD_CORE_TRUECALLERSYSTEM_H

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/AccountStatus.h"
#include "../enums/CallType.h"
#include "../enums/CallerTag.h"
#include "../enums/ReportReason.h"
#include "../models/CallLogEntry.h"
#include "../models/CallerLookupResult.h"
#include "../models/ContactEntry.h"
#include "../models/PhoneProfile.h"
#include "../models/User.h"
#include "../services/BlockService.h"
#include "../services/CallLogService.h"
#include "../services/ContactSyncService.h"
#include "../services/LookupService.h"
#include "../services/SearchService.h"
#include "../services/SpamReportService.h"
#include "../strategies/DefaultSpamScoringStrategy.h"
#include "../strategies/ISpamScoringStrategy.h"
#include "../utils/PhoneUtils.h"

namespace truecaller_lld {

class TruecallerSystem {
public:
    TruecallerSystem()
        : spamStrategy_(std::make_shared<DefaultSpamScoringStrategy>()),
          spamReportService_(spamStrategy_) {}

    std::string registerUser(const std::string& displayName, const std::string& phone,
                             const std::string& email = "", bool isBusiness = false) {
        const std::string normalized = normalizePhone(phone);
        if (phoneToUserId_.count(normalized) != 0) {
            throw std::runtime_error("phone already registered: " + normalized);
        }
        if (displayName.empty()) {
            throw std::invalid_argument("display name required");
        }

        const std::string userId = "TC_U" + std::to_string(++userCounter_);
        User user(userId, displayName, normalized, email, isBusiness);
        users_.emplace(userId, std::move(user));
        phoneToUserId_[normalized] = userId;

        CallerTag tag = isBusiness ? CallerTag::BUSINESS : CallerTag::PERSONAL;
        directory_[normalized] = PhoneProfile(normalized, displayName, userId, tag, 0);
        return userId;
    }

    void updateProfile(const std::string& userId, const std::string& displayName,
                       const std::string& email, bool isBusiness) {
        User& user = getUserOrThrow(userId);
        user.setDisplayName(displayName);
        user.setEmail(email);
        user.setBusiness(isBusiness);

        PhoneProfile& profile = getDirectoryProfileOrThrow(user.getPhone());
        profile.setDisplayName(displayName);
        profile.setPrimaryTag(isBusiness ? CallerTag::BUSINESS : CallerTag::PERSONAL);
    }

    CallerLookupResult identifyCaller(const std::string& forUserId, const std::string& callerPhone) {
        validateUser(forUserId);
        return lookupService_.lookup(forUserId, callerPhone, directory_, blockService_);
    }

    std::vector<ContactEntry> syncContacts(const std::string& userId,
                                             const std::vector<ContactEntry>& contacts) {
        validateUser(userId);
        syncedContacts_[userId] = contactSyncService_.syncContacts(contacts, directory_);
        return syncedContacts_[userId];
    }

    void reportSpam(const std::string& reporterUserId, const std::string& phone,
                    ReportReason reason) {
        validateUser(reporterUserId);
        spamReportService_.reportSpam(reporterUserId, phone, reason, directory_);
    }

    void blockNumber(const std::string& userId, const std::string& phone) {
        validateUser(userId);
        blockService_.block(userId, phone);
    }

    void unblockNumber(const std::string& userId, const std::string& phone) {
        validateUser(userId);
        blockService_.unblock(userId, phone);
    }

    std::vector<std::string> getBlockedNumbers(const std::string& userId) const {
        validateUser(userId);
        return blockService_.listBlocked(userId);
    }

    std::vector<PhoneProfile> searchByName(const std::string& prefix, size_t limit = 10) const {
        if (prefix.empty()) {
            throw std::invalid_argument("search prefix required");
        }
        return searchService_.searchByNamePrefix(prefix, directory_, limit);
    }

    std::vector<PhoneProfile> searchByPhone(const std::string& phone) const {
        return searchService_.searchByPhone(phone, directory_);
    }

    std::string logCall(const std::string& userId, const std::string& remotePhone, CallType type) {
        validateUser(userId);
        return callLogService_.logCall(userId, remotePhone, type);
    }

    std::vector<CallLogEntry> getCallHistory(const std::string& userId, size_t limit = 20) const {
        validateUser(userId);
        return callLogService_.getRecentCalls(userId, limit);
    }

    const User& getUser(const std::string& userId) const { return getUserOrThrow(userId); }

private:
    std::unordered_map<std::string, User> users_;
    std::unordered_map<std::string, std::string> phoneToUserId_;
    std::unordered_map<std::string, PhoneProfile> directory_;
    std::unordered_map<std::string, std::vector<ContactEntry>> syncedContacts_;

    BlockService blockService_;
    LookupService lookupService_;
    ContactSyncService contactSyncService_;
    SearchService searchService_;
    CallLogService callLogService_;
    std::shared_ptr<ISpamScoringStrategy> spamStrategy_;
    SpamReportService spamReportService_;

    int userCounter_{0};

    void validateUser(const std::string& userId) const {
        if (users_.find(userId) == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
    }

    User& getUserOrThrow(const std::string& userId) {
        auto it = users_.find(userId);
        if (it == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
        if (it->second.getStatus() != AccountStatus::ACTIVE) {
            throw std::runtime_error("user account suspended");
        }
        return it->second;
    }

    const User& getUserOrThrow(const std::string& userId) const {
        auto it = users_.find(userId);
        if (it == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
        return it->second;
    }

    PhoneProfile& getDirectoryProfileOrThrow(const std::string& phone) {
        auto it = directory_.find(phone);
        if (it == directory_.end()) {
            throw std::runtime_error("directory profile missing for " + phone);
        }
        return it->second;
    }
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_CORE_TRUECALLERSYSTEM_H
