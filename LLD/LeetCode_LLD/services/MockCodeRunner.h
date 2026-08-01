#ifndef LEETCODE_LLD_SERVICES_MOCKCODERUNNER_H
#define LEETCODE_LLD_SERVICES_MOCKCODERUNNER_H

#include <bits/stdc++.h>

#include "../algorithms/MinCostDivideArraySolver.h"
#include "../utils/TestInputParser.h"
#include "ICodeRunner.h"

namespace leetcode_lld {

using namespace std;

/**
 * Simulates code execution for LLD demo (no real compiler).
 * Recognizes solution markers in submitted code for seeded problems.
 */
class MockCodeRunner : public ICodeRunner {
public:
    string execute(const string &problemId,
                   const string &code,
                   ProgrammingLanguage language,
                   const string &input) const override {
        (void)language;

        if (code.find("COMPILE_ERROR") != string::npos) {
            throw runtime_error("Compilation failed");
        }
        if (code.find("RUNTIME_ERROR") != string::npos) {
            throw runtime_error("Runtime error");
        }
        if (code.find("TLE_MARKER") != string::npos) {
            throw runtime_error("Time limit exceeded");
        }

        if (problemId == "TWO_SUM") {
            return solveTwoSum(input, code);
        }
        if (problemId == "REVERSE_STRING") {
            return solveReverseString(input, code);
        }
        if (problemId == "VALID_PARENTHESES") {
            return solveValidParentheses(input, code);
        }
        if (problemId == "MIN_COST_DIVIDE_ARRAY") {
            return solveMinCostDivideArray(input, code);
        }

        return "WRONG";
    }

private:
    static string solveTwoSum(const string &input, const string &code) {
        if (code.find("SOLUTION_TWO_SUM") == string::npos) {
            return "WRONG";
        }
        // input format: nums=2,7,11,15;target=9
        if (input.find("target=9") != string::npos) {
            return "[0,1]";
        }
        if (input.find("target=6") != string::npos) {
            return "[1,2]";
        }
        return "WRONG";
    }

    static string solveReverseString(const string &input, const string &code) {
        if (code.find("SOLUTION_REVERSE") == string::npos) {
            return "WRONG";
        }
        const string s = input.substr(input.find('=') + 1);
        string reversed = s;
        reverse(reversed.begin(), reversed.end());
        return reversed;
    }

    static string solveValidParentheses(const string &input, const string &code) {
        if (code.find("SOLUTION_VALID_PAREN") == string::npos) {
            return "WRONG";
        }
        const string s = input.substr(input.find('=') + 1);
        stack<char> st;
        for (char ch : s) {
            if (ch == '(' || ch == '[' || ch == '{') {
                st.push(ch);
            } else if (ch == ')') {
                if (st.empty() || st.top() != '(') {
                    return "false";
                }
                st.pop();
            } else if (ch == ']') {
                if (st.empty() || st.top() != '[') {
                    return "false";
                }
                st.pop();
            } else if (ch == '}') {
                if (st.empty() || st.top() != '{') {
                    return "false";
                }
                st.pop();
            }
        }
        return st.empty() ? "true" : "false";
    }

    static string solveMinCostDivideArray(const string &input, const string &code) {
        vector<int> nums;
        vector<int> cost;
        int k = 0;
        parseMinCostDivideInput(input, nums, cost, k);

        const long long expected = MinCostDivideArraySolver::solve(nums, cost, k);

        if (code.find("SOLUTION_MIN_COST_DIVIDE") != string::npos) {
            return to_string(expected);
        }

        // Wrong submission returns off-by-one (for WA demo)
        return to_string(expected + 1);
    }
};

}  // namespace leetcode_lld

#endif
