#ifndef CRICBUZZ_LLD_ENUMS_MATCH_STATUS_H
#define CRICBUZZ_LLD_ENUMS_MATCH_STATUS_H

#include <string>

namespace cricbuzz_lld {

enum class MatchStatus { SCHEDULED, LIVE, COMPLETED, ABANDONED };

inline std::string matchStatusToString(MatchStatus status) {
    switch (status) {
        case MatchStatus::SCHEDULED:
            return "SCHEDULED";
        case MatchStatus::LIVE:
            return "LIVE";
        case MatchStatus::COMPLETED:
            return "COMPLETED";
        case MatchStatus::ABANDONED:
            return "ABANDONED";
    }
    return "UNKNOWN";
}

}  // namespace cricbuzz_lld

#endif
