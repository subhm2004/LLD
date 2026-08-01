// ============================================================================
//  services/RecommendationService.h  —  Professional recommendations (R6)
// ----------------------------------------------------------------------------
//  R6 (Recommendations): "Users can REQUEST and PROVIDE professional
//  recommendations, enhancing the credibility and value of their professional
//  network."
//
//  R6 me do actions hain, dono yahan handle hote hain:
//     REQUEST  -> "X" chahta hai ki "Y" uske liye recommendation likhe
//     PROVIDE  -> "Y" ne "X" ke liye recommendation likh di
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Recommendations "target" ke hisaab se store hoti hain                 │
//  │     (recommendationsByTarget_) — kyunki wo TARGET ke profile pe dikhti    │
//  │     hain. "Shubham ko 3 logon ne recommend kiya" = us target ki list.    │
//  │                                                                          │
//  │  ⭐ Jab koi PROVIDE karta hai, hum dekhte hain ki kya iske liye pehle     │
//  │     koi REQUEST pending thi — agar haan to use "fulfilled" maan ke        │
//  │     pending list se hata dete hain. (Request -> Provide ka natural flow.)│
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef LINKEDIN_LLD_SERVICES_RECOMMENDATIONSERVICE_H
#define LINKEDIN_LLD_SERVICES_RECOMMENDATIONSERVICE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "../models/Recommendation.h"

namespace linkedin_lld {

class RecommendationService {
public:
    // Ek pending recommendation request: "requester" chahta hai "author" likhe.
    struct PendingRequest {
        std::string requesterId; // jise recommendation CHAHIYE (target banega)
        std::string authorId;    // jisse LIKHWANI hai
    };

    // ---- R6 REQUEST: "author" se recommendation maango ---------------------
    void requestRecommendation(const std::string &requesterId, const std::string &authorId) {
        pendingRequests_.push_back({requesterId, authorId});
    }

    // ---- R6 PROVIDE: "author" ne "target" ke liye recommendation likhi -----
    std::string provideRecommendation(const std::string &authorId, const std::string &targetId,
                                      const std::string &text) {
        const std::string recId = "REC_" + std::to_string(++recommendationCounter_);
        recommendationsByTarget_[targetId].emplace_back(recId, authorId, targetId, text);

        // Agar target ne is author se request ki thi, wo ab poori ho gayi -> hatao.
        removePendingRequest(targetId, authorId);
        return recId;
    }

    // ---- Kisi user ke profile pe dikhne wali recommendations ---------------
    const std::vector<Recommendation> &getRecommendationsFor(const std::string &targetId) const {
        static const std::vector<Recommendation> empty;
        auto it = recommendationsByTarget_.find(targetId);
        return it == recommendationsByTarget_.end() ? empty : it->second;
    }

    // ---- Kisi author ke paas aayi pending recommendation requests -----------
    std::vector<PendingRequest> getPendingRequestsForAuthor(const std::string &authorId) const {
        std::vector<PendingRequest> result;
        for (const PendingRequest &req : pendingRequests_) {
            if (req.authorId == authorId) {
                result.push_back(req);
            }
        }
        return result;
    }

private:
    void removePendingRequest(const std::string &requesterId, const std::string &authorId) {
        for (auto it = pendingRequests_.begin(); it != pendingRequests_.end(); ++it) {
            if (it->requesterId == requesterId && it->authorId == authorId) {
                pendingRequests_.erase(it);
                return; // ek hi hata do (do same requests unlikely)
            }
        }
    }

    std::unordered_map<std::string, std::vector<Recommendation>> recommendationsByTarget_; // target -> recs
    std::vector<PendingRequest> pendingRequests_;                                          // R6 requests
    int recommendationCounter_ = 0;
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_SERVICES_RECOMMENDATIONSERVICE_H
