#ifndef LEETCODE_LLD_ENUMS_DIFFICULTY_H
#define LEETCODE_LLD_ENUMS_DIFFICULTY_H

#include <bits/stdc++.h>

namespace leetcode_lld {

using namespace std;

enum class Difficulty { EASY, MEDIUM, HARD };

inline string difficultyToString(Difficulty d) {
    switch (d) {
        case Difficulty::EASY:
            return "EASY";
        case Difficulty::MEDIUM:
            return "MEDIUM";
        case Difficulty::HARD:
            return "HARD";
    }
    return "UNKNOWN";
}

}  // namespace leetcode_lld

#endif
