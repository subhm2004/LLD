// ============================================================================
//  models/JobListing.h  —  Company page pe ek job opening (Requirement R12)
// ----------------------------------------------------------------------------
//  Requirement R12 (Job Listings): "Company pages can feature job openings,
//  enabling users to explore and apply for career opportunities."
//
//  Ek job listing ek CompanyPage ke andar rehti hai (CompanyPage.h dekho). Iske
//  paas ek applicants ki list hoti hai — jin logon ne apply kiya.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `applicants_` ek SET hai (vector nahi) — duplicate apply rok deta hai │
//  │                                                                          │
//  │  Ek user ek job pe do baar apply na kar paaye — set apne aap ye ensure    │
//  │  karta hai. `apply()` false lautata hai agar pehle se apply kar chuka.    │
//  │  (Bilkul Post ke shares/reactions jaisi hi soch — duplicate = set.)       │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef LINKEDIN_LLD_MODELS_JOBLISTING_H
#define LINKEDIN_LLD_MODELS_JOBLISTING_H

#include <string>
#include <unordered_set>
#include <utility>

namespace linkedin_lld {

class JobListing {
public:
    JobListing(std::string jobId, std::string companyPageId, std::string title, std::string description)
        : jobId_(std::move(jobId)),
          companyPageId_(std::move(companyPageId)),
          title_(std::move(title)),
          description_(std::move(description)) {}

    const std::string &getJobId() const { return jobId_; }
    const std::string &getCompanyPageId() const { return companyPageId_; }
    const std::string &getTitle() const { return title_; }
    const std::string &getDescription() const { return description_; }

    // ---- R12: user apply karta hai (duplicate apply safe) ------------------
    // true  -> apply ho gaya
    // false -> pehle se apply kar chuka tha
    bool apply(const std::string &userId) { return applicants_.insert(userId).second; }

    bool hasApplied(const std::string &userId) const { return applicants_.count(userId) != 0; }
    int getApplicantCount() const { return static_cast<int>(applicants_.size()); }
    const std::unordered_set<std::string> &getApplicants() const { return applicants_; }

private:
    std::string jobId_;
    std::string companyPageId_;             // kis company page ka job
    std::string title_;                     // "Senior C++ Engineer"
    std::string description_;               // job details
    std::unordered_set<std::string> applicants_; // jinhone apply kiya (R12)
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_MODELS_JOBLISTING_H
