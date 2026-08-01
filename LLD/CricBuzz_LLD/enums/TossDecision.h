#ifndef CRICBUZZ_LLD_ENUMS_TOSS_DECISION_H
#define CRICBUZZ_LLD_ENUMS_TOSS_DECISION_H

#include <string>

namespace cricbuzz_lld {

enum class TossDecision { BAT, BOWL };

inline std::string tossDecisionToString(TossDecision decision) {
    return decision == TossDecision::BAT ? "BAT" : "BOWL";
}

}  // namespace cricbuzz_lld

#endif
