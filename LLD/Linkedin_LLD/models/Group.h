// ============================================================================
//  models/Group.h  —  Ek professional group/community (Requirement R13)
// ----------------------------------------------------------------------------
//  Requirement R13 (Groups): "Users have the option to create and join
//  professional groups, encouraging the exchange of ideas and networking within
//  specific interest areas or industries."
//
//  Ek group: ek naam, ek creator, aur members ki list. Log join karte hain.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Creator apne aap pehla member ban jaata hai                          │
//  │                                                                          │
//  │  Constructor me hi `members_.insert(creatorId)` — kyunki jisne group     │
//  │  banaya wo obviously usme hai. Isse "creator group ka member hai ya      │
//  │  nahi" wali ajeeb haalat kabhi banti hi nahi.                            │
//  │                                                                          │
//  │  `members_` set hai -> ek user do baar join na kar paaye (duplicate safe)│
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef LINKEDIN_LLD_MODELS_GROUP_H
#define LINKEDIN_LLD_MODELS_GROUP_H

#include <string>
#include <unordered_set>
#include <utility>

namespace linkedin_lld {

class Group {
public:
    Group(std::string groupId, std::string name, std::string creatorId)
        : groupId_(std::move(groupId)), name_(std::move(name)), creatorId_(creatorId) {
        members_.insert(creatorId); // creator khud pehla member (upar note dekho)
    }

    const std::string &getGroupId() const { return groupId_; }
    const std::string &getName() const { return name_; }
    const std::string &getCreatorId() const { return creatorId_; }

    // ---- R13: group join karo (duplicate join safe) ------------------------
    bool join(const std::string &userId) { return members_.insert(userId).second; }

    bool isMember(const std::string &userId) const { return members_.count(userId) != 0; }
    int getMemberCount() const { return static_cast<int>(members_.size()); }

private:
    std::string groupId_;
    std::string name_;                       // "C++ Developers India"
    std::string creatorId_;
    std::unordered_set<std::string> members_; // R13 (creator include)
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_MODELS_GROUP_H
