#ifndef CRICBUZZ_LLD_ENUMS_BALL_OUTCOME_H
#define CRICBUZZ_LLD_ENUMS_BALL_OUTCOME_H

#include <string>

namespace cricbuzz_lld {

enum class BallOutcome { DOT, RUNS, FOUR, SIX, WIDE, NO_BALL, WICKET };

inline std::string ballOutcomeToString(BallOutcome outcome) {
    switch (outcome) {
        case BallOutcome::DOT:
            return "DOT";
        case BallOutcome::RUNS:
            return "RUNS";
        case BallOutcome::FOUR:
            return "FOUR";
        case BallOutcome::SIX:
            return "SIX";
        case BallOutcome::WIDE:
            return "WIDE";
        case BallOutcome::NO_BALL:
            return "NO_BALL";
        case BallOutcome::WICKET:
            return "WICKET";
    }
    return "UNKNOWN";
}

}  // namespace cricbuzz_lld

#endif
