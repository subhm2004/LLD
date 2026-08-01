// ============================================================================
//  services/SearchService.h  —  Users / Pages / Groups dhoondho (Requirement R2)
// ----------------------------------------------------------------------------
//  R2 (Search and Exploration): "Users have the capability to search for and
//  view other users' profiles, pages, and groups, fostering networking and
//  community engagement."
//
//  Ek keyword lo, teeno cheezon me dhoondho (users, company pages, groups), aur
//  matched IDs alag-alag list me lauta do.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ R5 tie-in: jab koi user search result me AATA hai, uski               │
//  │     "search appearances" count badh jaati hai                            │
//  │                                                                          │
//  │  Isi liye SearchService `users` ko NON-CONST leti hai — taaki matched     │
//  │  user pe `incrementSearchAppearances()` bula sake. "Aap is hafte 12 baar  │
//  │  search me aaye" wala insight yahin se banta hai (R5).                    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  💡 Matching CASE-INSENSITIVE substring hai — "eng" likho to "Backend
//     Engineer" bhi mile. Chhota `containsIgnoreCase` helper isi ke liye hai.
//     ⚠ Ye linear scan (O(N)) hai — asli system me inverted index / search
//     engine (Elasticsearch) hota. Demo scale pe theek hai.
// ============================================================================
#ifndef LINKEDIN_LLD_SERVICES_SEARCHSERVICE_H
#define LINKEDIN_LLD_SERVICES_SEARCHSERVICE_H

#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/CompanyPage.h"
#include "../models/Group.h"
#include "../models/UserProfile.h"

namespace linkedin_lld {

class SearchService {
public:
    // Ek search ka natija — teeno category alag-alag.
    struct SearchResult {
        std::vector<std::string> userIds;  // matched user IDs
        std::vector<std::string> pageIds;  // matched company page IDs
        std::vector<std::string> groupIds; // matched group IDs
    };

    SearchResult search(const std::string &keyword,
                        std::unordered_map<std::string, UserProfile> &users,
                        const std::unordered_map<std::string, CompanyPage> &pages,
                        const std::unordered_map<std::string, Group> &groups) const {
        SearchResult result;

        // ---- Users: name ya headline me keyword? -----------------------------
        for (auto &entry : users) {
            UserProfile &profile = entry.second;
            if (containsIgnoreCase(profile.getName(), keyword) ||
                containsIgnoreCase(profile.getHeadline(), keyword)) {
                result.userIds.push_back(profile.getUserId());
                profile.incrementSearchAppearances(); // R5 — search me aaya
            }
        }

        // ---- Company pages: name me keyword? --------------------------------
        for (const auto &entry : pages) {
            if (containsIgnoreCase(entry.second.getName(), keyword)) {
                result.pageIds.push_back(entry.second.getPageId());
            }
        }

        // ---- Groups: name me keyword? ---------------------------------------
        for (const auto &entry : groups) {
            if (containsIgnoreCase(entry.second.getName(), keyword)) {
                result.groupIds.push_back(entry.second.getGroupId());
            }
        }

        return result;
    }

private:
    // Case-insensitive "haystack me needle hai?" — dono ko lowercase karke dhoondo.
    static bool containsIgnoreCase(const std::string &haystack, const std::string &needle) {
        if (needle.empty()) {
            return false;
        }
        const std::string h = toLower(haystack);
        const std::string n = toLower(needle);
        return h.find(n) != std::string::npos;
    }

    static std::string toLower(const std::string &s) {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return out;
    }
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_SERVICES_SEARCHSERVICE_H
