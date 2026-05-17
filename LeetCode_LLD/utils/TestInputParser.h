#ifndef LEETCODE_LLD_UTILS_TESTINPUTPARSER_H
#define LEETCODE_LLD_UTILS_TESTINPUTPARSER_H

#include <bits/stdc++.h>

namespace leetcode_lld {

using namespace std;

inline vector<int> parseIntList(const string &raw) {
    vector<int> values;
    string token;
    stringstream stream(raw);
    while (getline(stream, token, ',')) {
        if (!token.empty()) {
            values.push_back(stoi(token));
        }
    }
    return values;
}

inline void parseMinCostDivideInput(const string &input,
                                    vector<int> &nums,
                                    vector<int> &cost,
                                    int &k) {
    nums.clear();
    cost.clear();
    k = 0;

    stringstream stream(input);
    string part;
    while (getline(stream, part, ';')) {
        const size_t equalPos = part.find('=');
        if (equalPos == string::npos) {
            continue;
        }
        const string key = part.substr(0, equalPos);
        const string value = part.substr(equalPos + 1);
        if (key == "nums") {
            nums = parseIntList(value);
        } else if (key == "cost") {
            cost = parseIntList(value);
        } else if (key == "k") {
            k = stoi(value);
        }
    }

    if (nums.empty() || nums.size() != cost.size() || k <= 0) {
        throw invalid_argument("Invalid MIN_COST_DIVIDE input: " + input);
    }
}

}  // namespace leetcode_lld

#endif
