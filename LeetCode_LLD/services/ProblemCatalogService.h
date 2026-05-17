#ifndef LEETCODE_LLD_SERVICES_PROBLEMCATALOGSERVICE_H
#define LEETCODE_LLD_SERVICES_PROBLEMCATALOGSERVICE_H

#include <bits/stdc++.h>

#include "../enums/Difficulty.h"
#include "../models/Problem.h"

namespace leetcode_lld {

using namespace std;

class ProblemCatalogService {
public:
    void addProblem(const Problem &problem) {
        if (problems_.count(problem.getProblemId())) {
            throw runtime_error("Problem already exists: " + problem.getProblemId());
        }
        problems_.emplace(problem.getProblemId(), problem);
    }

    Problem &getProblem(const string &problemId) {
        return getProblemOrThrow(problemId);
    }

    const Problem &getProblem(const string &problemId) const {
        return getProblemOrThrow(problemId);
    }

    vector<Problem *> searchByTag(const string &tag) const {
        vector<Problem *> result;
        for (const auto &entry : problems_) {
            if (entry.second.hasTag(tag)) {
                result.push_back(const_cast<Problem *>(&entry.second));
            }
        }
        return result;
    }

    vector<Problem *> searchByDifficulty(Difficulty difficulty) const {
        vector<Problem *> result;
        for (const auto &entry : problems_) {
            if (entry.second.getDifficulty() == difficulty) {
                result.push_back(const_cast<Problem *>(&entry.second));
            }
        }
        return result;
    }

    vector<Problem *> listAll() const {
        vector<Problem *> result;
        for (const auto &entry : problems_) {
            result.push_back(const_cast<Problem *>(&entry.second));
        }
        return result;
    }

private:
    Problem &getProblemOrThrow(const string &problemId) {
        const auto iterator = problems_.find(problemId);
        if (iterator == problems_.end()) {
            throw runtime_error("Problem not found: " + problemId);
        }
        return iterator->second;
    }

    const Problem &getProblemOrThrow(const string &problemId) const {
        const auto iterator = problems_.find(problemId);
        if (iterator == problems_.end()) {
            throw runtime_error("Problem not found: " + problemId);
        }
        return iterator->second;
    }

    unordered_map<string, Problem> problems_;
};

}  // namespace leetcode_lld

#endif
