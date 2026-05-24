#ifndef CRICBUZZ_LLD_MODELS_MATCH_H
#define CRICBUZZ_LLD_MODELS_MATCH_H

#include <string>
#include <vector>

#include "../enums/MatchFormat.h"
#include "../enums/MatchStatus.h"
#include "../enums/TossDecision.h"
#include "../models/InningsScore.h"

namespace cricbuzz_lld {

struct Match {
    std::string matchId;
    std::string teamAId;
    std::string teamBId;
    std::string venue;
    MatchFormat format{MatchFormat::T20};
    MatchStatus status{MatchStatus::SCHEDULED};
    std::string tossWinnerTeamId;
    TossDecision tossDecision{TossDecision::BAT};
    int currentInningsIndex{0};
    std::vector<InningsScore> innings;
    std::string resultSummary;
};

}  // namespace cricbuzz_lld

#endif
