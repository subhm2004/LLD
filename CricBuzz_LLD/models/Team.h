#ifndef CRICBUZZ_LLD_MODELS_TEAM_H
#define CRICBUZZ_LLD_MODELS_TEAM_H

#include <string>

namespace cricbuzz_lld {

struct Team {
    std::string teamId;
    std::string name;
    std::string shortCode;
};

}  // namespace cricbuzz_lld

#endif
