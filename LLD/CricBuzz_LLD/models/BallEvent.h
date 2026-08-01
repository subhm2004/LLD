#ifndef CRICBUZZ_LLD_MODELS_BALL_EVENT_H
#define CRICBUZZ_LLD_MODELS_BALL_EVENT_H

#include <string>

#include "../enums/BallOutcome.h"

namespace cricbuzz_lld {

struct BallEvent {
    std::string eventId;
    std::string matchId;
    int inningsIndex{0};
    int overNumber{0};
    int ballInOver{0};  // 1-6 for legal; 0 for extras marker
    std::string batsmanId;
    std::string bowlerId;
    BallOutcome outcome{BallOutcome::DOT};
    int runsScored{0};
    bool isWicket{false};
    std::string wicketType;
    std::string commentary;
};

}  // namespace cricbuzz_lld

#endif
