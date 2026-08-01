#ifndef CRICBUZZ_LLD_ENUMS_MATCH_FORMAT_H
#define CRICBUZZ_LLD_ENUMS_MATCH_FORMAT_H

#include <string>

namespace cricbuzz_lld {

enum class MatchFormat { T20, ODI, TEST };

inline std::string matchFormatToString(MatchFormat format) {
    switch (format) {
        case MatchFormat::T20:
            return "T20";
        case MatchFormat::ODI:
            return "ODI";
        case MatchFormat::TEST:
            return "TEST";
    }
    return "UNKNOWN";
}

inline int maxOversForFormat(MatchFormat format) {
    switch (format) {
        case MatchFormat::T20:
            return 20;
        case MatchFormat::ODI:
            return 50;
        case MatchFormat::TEST:
            return 90;  // per innings cap for demo
    }
    return 20;
}

}  // namespace cricbuzz_lld

#endif
