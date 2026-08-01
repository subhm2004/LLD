#ifndef LEETCODE_LLD_MODELS_SUBMISSION_H
#define LEETCODE_LLD_MODELS_SUBMISSION_H

#include <bits/stdc++.h>

#include "../enums/ProgrammingLanguage.h"
#include "../enums/SubmissionStatus.h"

namespace leetcode_lld {

using namespace std;

class Submission {
public:
    Submission(string submissionId,
               string userId,
               string problemId,
               ProgrammingLanguage language,
               string code)
        : submissionId_(std::move(submissionId)),
          userId_(std::move(userId)),
          problemId_(std::move(problemId)),
          language_(language),
          code_(std::move(code)),
          status_(SubmissionStatus::PENDING),
          runtimeMs_(0),
          memoryKb_(0) {}

    const string &getSubmissionId() const { return submissionId_; }
    const string &getUserId() const { return userId_; }
    const string &getProblemId() const { return problemId_; }
    ProgrammingLanguage getLanguage() const { return language_; }
    const string &getCode() const { return code_; }

    SubmissionStatus getStatus() const { return status_; }
    int getRuntimeMs() const { return runtimeMs_; }
    int getMemoryKb() const { return memoryKb_; }
    const string &getFailedTestCaseId() const { return failedTestCaseId_; }

    void setResult(SubmissionStatus status, int runtimeMs, int memoryKb, string failedTestCaseId = "") {
        status_ = status;
        runtimeMs_ = runtimeMs;
        memoryKb_ = memoryKb;
        failedTestCaseId_ = std::move(failedTestCaseId);
    }

private:
    string submissionId_;
    string userId_;
    string problemId_;
    ProgrammingLanguage language_;
    string code_;
    SubmissionStatus status_;
    int runtimeMs_;
    int memoryKb_;
    string failedTestCaseId_;
};

}  // namespace leetcode_lld

#endif
