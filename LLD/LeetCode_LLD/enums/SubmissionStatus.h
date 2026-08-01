#ifndef LEETCODE_LLD_ENUMS_SUBMISSIONSTATUS_H
#define LEETCODE_LLD_ENUMS_SUBMISSIONSTATUS_H

#include <bits/stdc++.h>

namespace leetcode_lld {

using namespace std;

enum class SubmissionStatus {
    PENDING,
    ACCEPTED,
    WRONG_ANSWER,
    TIME_LIMIT_EXCEEDED,
    RUNTIME_ERROR,
    COMPILE_ERROR
};

inline string statusToString(SubmissionStatus s) {
    switch (s) {
        case SubmissionStatus::PENDING:
            return "PENDING";
        case SubmissionStatus::ACCEPTED:
            return "ACCEPTED";
        case SubmissionStatus::WRONG_ANSWER:
            return "WRONG_ANSWER";
        case SubmissionStatus::TIME_LIMIT_EXCEEDED:
            return "TIME_LIMIT_EXCEEDED";
        case SubmissionStatus::RUNTIME_ERROR:
            return "RUNTIME_ERROR";
        case SubmissionStatus::COMPILE_ERROR:
            return "COMPILE_ERROR";
    }
    return "UNKNOWN";
}

}  // namespace leetcode_lld

#endif
