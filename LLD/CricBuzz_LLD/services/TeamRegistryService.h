#ifndef CRICBUZZ_LLD_SERVICES_TEAM_REGISTRY_SERVICE_H
#define CRICBUZZ_LLD_SERVICES_TEAM_REGISTRY_SERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../models/Player.h"
#include "../models/Team.h"

namespace cricbuzz_lld {

class TeamRegistryService {
public:
    std::string registerTeam(const std::string& name, const std::string& shortCode) {
        const std::string teamId = "team_" + std::to_string(++teamCounter_);
        teams_[teamId] = Team{teamId, name, shortCode};
        return teamId;
    }

    std::string registerPlayer(const std::string& teamId, const std::string& name) {
        getTeam(teamId);
        const std::string playerId = "ply_" + std::to_string(++playerCounter_);
        players_[playerId] = Player{playerId, name, teamId};
        return playerId;
    }

    const Team& getTeam(const std::string& teamId) const {
        auto it = teams_.find(teamId);
        if (it == teams_.end()) {
            throw std::invalid_argument("unknown team: " + teamId);
        }
        return it->second;
    }

    const Player& getPlayer(const std::string& playerId) const {
        auto it = players_.find(playerId);
        if (it == players_.end()) {
            throw std::invalid_argument("unknown player: " + playerId);
        }
        return it->second;
    }

private:
    std::unordered_map<std::string, Team> teams_;
    std::unordered_map<std::string, Player> players_;
    int teamCounter_{0};
    int playerCounter_{0};
};

}  // namespace cricbuzz_lld

#endif
