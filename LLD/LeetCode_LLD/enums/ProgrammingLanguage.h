#ifndef LEETCODE_LLD_ENUMS_PROGRAMMINGLANGUAGE_H
#define LEETCODE_LLD_ENUMS_PROGRAMMINGLANGUAGE_H

#include <bits/stdc++.h>

namespace leetcode_lld {

using namespace std;

enum class ProgrammingLanguage { CPP, JAVA, PYTHON };

inline string languageToString(ProgrammingLanguage lang) {
    switch (lang) {
        case ProgrammingLanguage::CPP:
            return "CPP";
        case ProgrammingLanguage::JAVA:
            return "JAVA";
        case ProgrammingLanguage::PYTHON:
            return "PYTHON";
    }
    return "UNKNOWN";
}

}  // namespace leetcode_lld

#endif
