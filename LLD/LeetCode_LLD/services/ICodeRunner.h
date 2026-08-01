#ifndef LEETCODE_LLD_SERVICES_ICODERUNNER_H
#define LEETCODE_LLD_SERVICES_ICODERUNNER_H

#include <bits/stdc++.h>

#include "../enums/ProgrammingLanguage.h"

namespace leetcode_lld {

using namespace std;

class ICodeRunner {
public:
    virtual ~ICodeRunner() = default;
    virtual string execute(const string &problemId,
                           const string &code,
                           ProgrammingLanguage language,
                           const string &input) const = 0;
};

}  // namespace leetcode_lld

#endif
