#include <bits/stdc++.h>

#include "core/LeetCodeSystem.h"

using namespace std;
using namespace leetcode_lld;

static void printSubmission(const Submission &submission) {
    cout << "  Submission " << submission.getSubmissionId() << " | Problem: " << submission.getProblemId()
         << " | Lang: " << languageToString(submission.getLanguage()) << " | Status: "
         << statusToString(submission.getStatus());
    if (!submission.getFailedTestCaseId().empty()) {
        cout << " | Failed TC: " << submission.getFailedTestCaseId();
    }
    if (submission.getStatus() == SubmissionStatus::ACCEPTED) {
        cout << " | Runtime: " << submission.getRuntimeMs() << "ms | Memory: " << submission.getMemoryKb() << "KB";
    }
    cout << "\n";
}

int main() {
    LeetCodeSystem system;
    system.seedDefaultProblems();

    system.registerUser("u1", "Shubham");
    system.registerUser("u2", "Ananya");
    system.registerUser("u3", "Ravi");

    cout << "=== Problems tagged 'array' ===\n";
    for (Problem *problem : system.searchProblemsByTag("array")) {
        cout << problem->getProblemId() << " | " << problem->getTitle() << " | "
             << difficultyToString(problem->getDifficulty()) << "\n";
    }

    cout << "\n=== EASY problems ===\n";
    for (Problem *problem : system.searchProblemsByDifficulty(Difficulty::EASY)) {
        cout << problem->getProblemId() << " | acceptance=" << problem->getAcceptanceRate() << "%\n";
    }

    cout << "\n=== HARD problems (DP) ===\n";
    for (Problem *problem : system.searchProblemsByDifficulty(Difficulty::HARD)) {
        cout << problem->getProblemId() << " | " << problem->getTitle() << "\n";
    }

    cout << "\n=== Submissions ===\n";

    const string correctTwoSum = R"(
// SOLUTION_TWO_SUM
class Solution { public: vector<int> twoSum(...) { return {0,1}; } };
)";
    const string wrongTwoSum = "// wrong approach\nreturn {1,2};";
    const string correctReverse = "// SOLUTION_REVERSE\nreverse in-place";
    const string compileFail = "// COMPILE_ERROR\nsyntax error here";
    const string tleCode = "// TLE_MARKER\nwhile(true){}";

    printSubmission(system.submitSolution("u1", "TWO_SUM", ProgrammingLanguage::CPP, correctTwoSum));
    printSubmission(system.submitSolution("u1", "TWO_SUM", ProgrammingLanguage::CPP, wrongTwoSum));
    printSubmission(system.submitSolution("u2", "REVERSE_STRING", ProgrammingLanguage::CPP, correctReverse));
    printSubmission(system.submitSolution("u2", "VALID_PARENTHESES", ProgrammingLanguage::CPP, "// SOLUTION_VALID_PAREN"));
    printSubmission(system.submitSolution("u3", "TWO_SUM", ProgrammingLanguage::JAVA, compileFail));
    printSubmission(system.submitSolution("u3", "REVERSE_STRING", ProgrammingLanguage::PYTHON, tleCode));

    const string correctMinCostDivide = R"(
// SOLUTION_MIN_COST_DIVIDE
// DP: dp[i] = min cost for prefix ending at i
// transition with subarray order = segments(l) + 1
class Solution {
public:
    long long minimumCost(vector<int>& nums, vector<int>& cost, int k) {
        // judge uses reference solver in MockCodeRunner
        return 0;
    }
};
)";
    const string wrongMinCostDivide = "// naive greedy — wrong\nreturn 0;";

    printSubmission(system.submitSolution("u1",
                                          "MIN_COST_DIVIDE_ARRAY",
                                          ProgrammingLanguage::CPP,
                                          correctMinCostDivide));
    printSubmission(system.submitSolution("u2",
                                          "MIN_COST_DIVIDE_ARRAY",
                                          ProgrammingLanguage::CPP,
                                          wrongMinCostDivide));

    cout << "\n=== User stats ===\n";
    cout << "u1 solved: " << system.getUser("u1").getSolvedCount() << "\n";
    cout << "u2 solved: " << system.getUser("u2").getSolvedCount() << "\n";
    cout << "u3 solved: " << system.getUser("u3").getSolvedCount() << "\n";

    cout << "\n=== Problem stats after submissions ===\n";
    const Problem &twoSum = system.getProblem("TWO_SUM");
    cout << "TWO_SUM acceptance: " << twoSum.getAcceptanceRate() << "% ("
         << twoSum.getTotalSubmissions() << " submissions)\n";

    cout << "\n=== Leaderboard (top solved) ===\n";
    int rank = 1;
    for (const auto &entry : system.getLeaderboard(5)) {
        cout << rank++ << ". " << entry.first << " — " << entry.second << " solved\n";
    }

    cout << "\n=== u1 submission history ===\n";
    for (const Submission &submission : system.getUserSubmissions("u1")) {
        printSubmission(submission);
    }

    cout << "\nLeetCode_LLD demo completed.\n";
    return 0;
}
