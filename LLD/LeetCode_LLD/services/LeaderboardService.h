#ifndef LEETCODE_LLD_SERVICES_LEADERBOARDSERVICE_H
#define LEETCODE_LLD_SERVICES_LEADERBOARDSERVICE_H

#include <bits/stdc++.h>

#include "../models/User.h"

namespace leetcode_lld {

using namespace std;

class LeaderboardService {
public:
    vector<pair<string, int>> getTopUsersBySolvedCount(const unordered_map<string, User> &users, int limit) const {
        vector<pair<string, int>> entries;
        entries.reserve(users.size());
        for (const auto &entry : users) {
            entries.emplace_back(entry.second.getDisplayName(), entry.second.getSolvedCount());
        }

        sort(entries.begin(), entries.end(), [](const pair<string, int> &a, const pair<string, int> &b) {
            if (a.second != b.second) {
                return a.second > b.second;
            }
            return a.first < b.first;
        });

        if (static_cast<int>(entries.size()) > limit) {
            entries.resize(static_cast<size_t>(limit));
        }
        return entries;
    }
};

}  // namespace leetcode_lld

#endif
