#ifndef CRICBUZZ_LLD_MODELS_LIVE_SCOREBOARD_H
#define CRICBUZZ_LLD_MODELS_LIVE_SCOREBOARD_H

#include <string>

#include "../enums/MatchStatus.h"

namespace cricbuzz_lld {

struct LiveScoreboard {
    std::string matchId;
    MatchStatus status{MatchStatus::SCHEDULED};
    std::string battingTeamName;
    std::string bowlingTeamName;
    int runs{0};
    int wickets{0};
    int overs{0};
    int ballsInOver{0};
    std::string runRate;
    std::string targetInfo;
    std::string lastBallSummary;
};

}  // namespace cricbuzz_lld

#endif
