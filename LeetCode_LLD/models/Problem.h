#ifndef LEETCODE_LLD_MODELS_PROBLEM_H
#define LEETCODE_LLD_MODELS_PROBLEM_H

#include <bits/stdc++.h>

#include "../enums/Difficulty.h"
#include "TestCase.h"

namespace leetcode_lld {

using namespace std;

class Problem {
public:
    Problem(string problemId, string title, Difficulty difficulty, vector<string> tags)
        : problemId_(std::move(problemId)),
          title_(std::move(title)),
          difficulty_(difficulty),
          tags_(std::move(tags)),
          acceptanceRate_(0.0),
          totalSubmissions_(0),
          acceptedSubmissions_(0) {}

    void addTestCase(const TestCase &testCase) { testCases_.push_back(testCase); }

    const string &getProblemId() const { return problemId_; }
    const string &getTitle() const { return title_; }
    Difficulty getDifficulty() const { return difficulty_; }
    const vector<string> &getTags() const { return tags_; }
    const vector<TestCase> &getTestCases() const { return testCases_; }

    double getAcceptanceRate() const { return acceptanceRate_; }
    int getTotalSubmissions() const { return totalSubmissions_; }

    void recordSubmission(bool accepted) {
        ++totalSubmissions_;
        if (accepted) {
            ++acceptedSubmissions_;
        }
        if (totalSubmissions_ > 0) {
            acceptanceRate_ = (100.0 * acceptedSubmissions_) / totalSubmissions_;
        }
    }

    bool hasTag(const string &tag) const {
        return find(tags_.begin(), tags_.end(), tag) != tags_.end();
    }

private:
    string problemId_;
    string title_;
    Difficulty difficulty_;
    vector<string> tags_;
    vector<TestCase> testCases_;
    double acceptanceRate_;
    int totalSubmissions_;
    int acceptedSubmissions_{0};
};

}  // namespace leetcode_lld

#endif
