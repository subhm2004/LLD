#ifndef LEETCODE_LLD_SERVICES_SUBMISSIONSERVICE_H
#define LEETCODE_LLD_SERVICES_SUBMISSIONSERVICE_H

#include <bits/stdc++.h>

#include "../models/Submission.h"
#include "../models/User.h"
#include "JudgeService.h"
#include "ProblemCatalogService.h"

namespace leetcode_lld {

using namespace std;

class SubmissionService {
public:
    SubmissionService(ProblemCatalogService *catalog, JudgeService *judge)
        : catalog_(catalog), judge_(judge), submissionCounter_(0) {
        if (catalog_ == nullptr || judge_ == nullptr) {
            throw invalid_argument("Catalog and Judge services are required");
        }
    }

    Submission submit(const string &userId,
                      const string &problemId,
                      ProgrammingLanguage language,
                      const string &code,
                      User &user) {
        Problem &problem = catalog_->getProblem(problemId);
        const string submissionId = "SUB_" + to_string(++submissionCounter_);

        Submission submission(submissionId, userId, problemId, language, code);
        judge_->judge(submission, problem);

        submissions_.emplace(submissionId, submission);
        problem.recordSubmission(submission.getStatus() == SubmissionStatus::ACCEPTED);

        if (submission.getStatus() == SubmissionStatus::ACCEPTED) {
            user.markProblemSolved(problemId);
        }

        return submission;
    }

    const Submission &getSubmission(const string &submissionId) const {
        const auto iterator = submissions_.find(submissionId);
        if (iterator == submissions_.end()) {
            throw runtime_error("Submission not found: " + submissionId);
        }
        return iterator->second;
    }

    vector<Submission> getSubmissionsForUser(const string &userId) const {
        vector<Submission> result;
        for (const auto &entry : submissions_) {
            if (entry.second.getUserId() == userId) {
                result.push_back(entry.second);
            }
        }
        return result;
    }

private:
    ProblemCatalogService *catalog_;
    JudgeService *judge_;
    int submissionCounter_;
    unordered_map<string, Submission> submissions_;
};

}  // namespace leetcode_lld

#endif
