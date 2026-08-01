#ifndef LEETCODE_LLD_SERVICES_JUDGESERVICE_H
#define LEETCODE_LLD_SERVICES_JUDGESERVICE_H

#include <bits/stdc++.h>

#include "../enums/SubmissionStatus.h"
#include "../models/Problem.h"
#include "../models/Submission.h"
#include "ICodeRunner.h"

namespace leetcode_lld {

using namespace std;

class JudgeService {
public:
    explicit JudgeService(const ICodeRunner *codeRunner) : codeRunner_(codeRunner) {
        if (codeRunner_ == nullptr) {
            throw invalid_argument("CodeRunner cannot be null");
        }
    }

    void judge(Submission &submission, const Problem &problem) const {
        const int timeLimitMs = 2000;

        for (const TestCase &testCase : problem.getTestCases()) {
            try {
                const auto start = chrono::steady_clock::now();

                const string actualOutput =
                    codeRunner_->execute(problem.getProblemId(),
                                         submission.getCode(),
                                         submission.getLanguage(),
                                         testCase.getInput());

                const auto elapsed =
                    chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count();

                if (elapsed > timeLimitMs) {
                    submission.setResult(SubmissionStatus::TIME_LIMIT_EXCEEDED, static_cast<int>(elapsed), 0,
                                         testCase.getId());
                    return;
                }

                if (actualOutput != testCase.getExpectedOutput()) {
                    submission.setResult(SubmissionStatus::WRONG_ANSWER, static_cast<int>(elapsed), 128,
                                         testCase.getId());
                    return;
                }
            } catch (const exception &) {
                if (submission.getCode().find("COMPILE_ERROR") != string::npos) {
                    submission.setResult(SubmissionStatus::COMPILE_ERROR, 0, 0, testCase.getId());
                } else if (submission.getCode().find("TLE_MARKER") != string::npos) {
                    submission.setResult(SubmissionStatus::TIME_LIMIT_EXCEEDED, timeLimitMs + 1, 0, testCase.getId());
                } else {
                    submission.setResult(SubmissionStatus::RUNTIME_ERROR, 0, 0, testCase.getId());
                }
                return;
            }
        }

        submission.setResult(SubmissionStatus::ACCEPTED, 42, 1024);
    }

private:
    const ICodeRunner *codeRunner_;
};

}  // namespace leetcode_lld

#endif
