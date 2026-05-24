#ifndef CRICBUZZ_LLD_MODELS_INNINGS_SCORE_H
#define CRICBUZZ_LLD_MODELS_INNINGS_SCORE_H

#include <string>

namespace cricbuzz_lld {

struct InningsScore {
    std::string battingTeamId;
    std::string bowlingTeamId;
    int runs{0};
    int wickets{0};
    int legalBalls{0};  // total legal deliveries
    bool completed{false};
};

}  // namespace cricbuzz_lld

#endif
