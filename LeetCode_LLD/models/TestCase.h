#ifndef LEETCODE_LLD_MODELS_TESTCASE_H
#define LEETCODE_LLD_MODELS_TESTCASE_H

#include <bits/stdc++.h>

namespace leetcode_lld {

using namespace std;

class TestCase {
public:
    TestCase(string id, string input, string expectedOutput, bool isSample = true)
        : id_(std::move(id)),
          input_(std::move(input)),
          expectedOutput_(std::move(expectedOutput)),
          isSample_(isSample) {}

    const string &getId() const { return id_; }
    const string &getInput() const { return input_; }
    const string &getExpectedOutput() const { return expectedOutput_; }
    bool isSample() const { return isSample_; }

private:
    string id_;
    string input_;
    string expectedOutput_;
    bool isSample_;
};

}  // namespace leetcode_lld

#endif
