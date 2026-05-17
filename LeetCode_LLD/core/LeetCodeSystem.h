#ifndef LEETCODE_LLD_CORE_LEETCODESYSTEM_H
#define LEETCODE_LLD_CORE_LEETCODESYSTEM_H

#include <bits/stdc++.h>

#include "../models/Problem.h"
#include "../models/Submission.h"
#include "../models/TestCase.h"
#include "../models/User.h"
#include "../services/JudgeService.h"
#include "../services/LeaderboardService.h"
#include "../services/MockCodeRunner.h"
#include "../services/ProblemCatalogService.h"
#include "../services/SubmissionService.h"

namespace leetcode_lld {

using namespace std;

class LeetCodeSystem {
public:
    LeetCodeSystem()
        : codeRunner_(),
          judgeService_(&codeRunner_),
          submissionService_(&catalogService_, &judgeService_) {}

    void registerUser(const string &userId, const string &displayName) {
        if (users_.count(userId)) {
            throw runtime_error("User already exists: " + userId);
        }
        users_.emplace(userId, User(userId, displayName));
    }

    void addProblem(const Problem &problem) { catalogService_.addProblem(problem); }

    vector<Problem *> searchProblemsByTag(const string &tag) const {
        return catalogService_.searchByTag(tag);
    }

    vector<Problem *> searchProblemsByDifficulty(Difficulty difficulty) const {
        return catalogService_.searchByDifficulty(difficulty);
    }

    Submission submitSolution(const string &userId,
                              const string &problemId,
                              ProgrammingLanguage language,
                              const string &code) {
        return submissionService_.submit(userId, problemId, language, code, getUserOrThrow(userId));
    }

    const Submission &getSubmission(const string &submissionId) const {
        return submissionService_.getSubmission(submissionId);
    }

    vector<Submission> getUserSubmissions(const string &userId) const {
        return submissionService_.getSubmissionsForUser(userId);
    }

    vector<pair<string, int>> getLeaderboard(int limit = 10) const {
        return leaderboardService_.getTopUsersBySolvedCount(users_, limit);
    }

    const Problem &getProblem(const string &problemId) const {
        return catalogService_.getProblem(problemId);
    }

    const User &getUser(const string &userId) const { return getUserOrThrow(userId); }

    void seedDefaultProblems() {
        Problem twoSum("TWO_SUM", "Two Sum", Difficulty::EASY, {"array", "hash-table"});
        twoSum.addTestCase(TestCase("TC1", "nums=2,7,11,15;target=9", "[0,1]", true));
        twoSum.addTestCase(TestCase("TC2", "nums=3,2,4;target=6", "[1,2]", false));
        addProblem(twoSum);

        Problem reverseString("REVERSE_STRING", "Reverse String", Difficulty::EASY, {"string", "two-pointers"});
        reverseString.addTestCase(TestCase("TC1", "s=hello", "olleh", true));
        reverseString.addTestCase(TestCase("TC2", "s=world", "dlrow", false));
        addProblem(reverseString);

        Problem validParen("VALID_PARENTHESES", "Valid Parentheses", Difficulty::MEDIUM, {"stack", "string"});
        validParen.addTestCase(TestCase("TC1", "s=()", "true", true));
        validParen.addTestCase(TestCase("TC2", "s=(]", "false", false));
        validParen.addTestCase(TestCase("TC3", "s=([)]", "false", false));
        addProblem(validParen);

        Problem minCostDivide("MIN_COST_DIVIDE_ARRAY",
                              "Minimum Cost to Divide Array Into Subarrays",
                              Difficulty::HARD,
                              {"array", "dynamic-programming", "prefix-sum"});
        minCostDivide.addTestCase(
            TestCase("TC1", "nums=3,1,4;cost=4,6,6;k=1", "110", true));
        minCostDivide.addTestCase(TestCase(
            "TC2",
            "nums=4,8,5,1,14,2,2,12,1;cost=7,2,8,4,2,2,1,1,2;k=7",
            "985",
            true));
        minCostDivide.addTestCase(
            TestCase("TC3", "nums=1;cost=5;k=1", "10", false));
        addProblem(minCostDivide);
    }

private:
    User &getUserOrThrow(const string &userId) {
        const auto iterator = users_.find(userId);
        if (iterator == users_.end()) {
            throw runtime_error("User not found: " + userId);
        }
        return iterator->second;
    }

    const User &getUserOrThrow(const string &userId) const {
        const auto iterator = users_.find(userId);
        if (iterator == users_.end()) {
            throw runtime_error("User not found: " + userId);
        }
        return iterator->second;
    }

    MockCodeRunner codeRunner_;
    JudgeService judgeService_;
    ProblemCatalogService catalogService_;
    SubmissionService submissionService_;
    LeaderboardService leaderboardService_;
    unordered_map<string, User> users_;
};

}  // namespace leetcode_lld

#endif
