#ifndef CRICBUZZ_LLD_MODELS_PLAYER_H
#define CRICBUZZ_LLD_MODELS_PLAYER_H

#include <string>

namespace cricbuzz_lld {

struct Player {
    std::string playerId;
    std::string name;
    std::string teamId;
};

}  // namespace cricbuzz_lld

#endif
