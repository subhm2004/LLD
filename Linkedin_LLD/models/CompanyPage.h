// ============================================================================
//  models/CompanyPage.h  —  Ek company ka page (Requirement R11, R12)
// ----------------------------------------------------------------------------
//  R11 (Company Pages): "Users can create pages for companies and follow other
//                        company pages, connecting them with corporate entities."
//  R12 (Job Listings):  "Company pages can feature job openings..."
//
//  Ek company page ke paas:
//     - ek admin (jisne banaya)
//     - followers ki list  (R11 — log page follow karte hain)
//     - job listings ki list (R12 — page pe jobs)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CompanyPage job listings ko VALUE me OWN karta hai (vector<JobListing>)│
//  │                                                                          │
//  │  Job ek company page ke bina akele exist nahi karti — uski zindagi page  │
//  │  se bandhi hai. Isi liye page unhe apne andar rakhta hai (composition).  │
//  │  Page delete -> uske jobs bhi gaye. Ye asli duniya se match karta hai.   │
//  │                                                                          │
//  │  💡 `JobListing*` (pointer) return karte hain taaki caller us job pe      │
//  │     `apply()` bula sake (asli object modify ho, copy nahi).              │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef LINKEDIN_LLD_MODELS_COMPANYPAGE_H
#define LINKEDIN_LLD_MODELS_COMPANYPAGE_H

#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "JobListing.h"

namespace linkedin_lld {

class CompanyPage {
public:
    CompanyPage(std::string pageId, std::string name, std::string adminId)
        : pageId_(std::move(pageId)), name_(std::move(name)), adminId_(std::move(adminId)) {}

    const std::string &getPageId() const { return pageId_; }
    const std::string &getName() const { return name_; }
    const std::string &getAdminId() const { return adminId_; } // jisne page banaya

    // ---- R11: page ko follow karo (duplicate follow safe) ------------------
    bool addFollower(const std::string &userId) { return followers_.insert(userId).second; }
    bool isFollowedBy(const std::string &userId) const { return followers_.count(userId) != 0; }
    int getFollowerCount() const { return static_cast<int>(followers_.size()); }

    // ---- R12: page pe job listing daalo ------------------------------------
    void addJobListing(const JobListing &job) { jobListings_.push_back(job); }
    const std::vector<JobListing> &getJobListings() const { return jobListings_; }

    // Job ko id se dhoondho (apply karne ke liye chahiye). Nahi mila -> nullptr.
    JobListing *findJob(const std::string &jobId) {
        for (JobListing &job : jobListings_) {
            if (job.getJobId() == jobId) {
                return &job;
            }
        }
        return nullptr;
    }

private:
    std::string pageId_;
    std::string name_;                        // "Google", "Netflix"
    std::string adminId_;                     // page ka creator/admin
    std::unordered_set<std::string> followers_; // R11
    std::vector<JobListing> jobListings_;     // R12 (page inhe own karta hai)
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_MODELS_COMPANYPAGE_H
