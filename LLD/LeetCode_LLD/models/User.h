#ifndef LEETCODE_LLD_MODELS_USER_H
#define LEETCODE_LLD_MODELS_USER_H

#include <bits/stdc++.h>

namespace leetcode_lld {

using namespace std;

class User {
public:
    User(string userId, string displayName)
        : userId_(std::move(userId)), displayName_(std::move(displayName)), solvedCount_(0) {}

    const string &getUserId() const { return userId_; }
    const string &getDisplayName() const { return displayName_; }
    int getSolvedCount() const { return solvedCount_; }

    void incrementSolvedCount() { ++solvedCount_; }

    void markProblemSolved(const string &problemId) {
        if (solvedProblemIds_.insert(problemId).second) {
            incrementSolvedCount();
        }
    }

    bool hasSolved(const string &problemId) const {
        return solvedProblemIds_.count(problemId) > 0;
    }

private:
    string userId_;
    string displayName_;
    int solvedCount_;
    unordered_set<string> solvedProblemIds_;
};

}  // namespace leetcode_lld

#endif
