#ifndef CRICBUZZ_LLD_SERVICES_MATCH_SERVICE_H
#define CRICBUZZ_LLD_SERVICES_MATCH_SERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/MatchFormat.h"
#include "../enums/MatchStatus.h"
#include "../enums/TossDecision.h"
#include "../models/InningsScore.h"
#include "../models/Match.h"
#include "TeamRegistryService.h"

namespace cricbuzz_lld {

class MatchService {
public:
    explicit MatchService(TeamRegistryService* teams) : teams_(teams) {}

    std::string scheduleMatch(const std::string& teamAId, const std::string& teamBId,
                              const std::string& venue, MatchFormat format) {
        if (teamAId == teamBId) {
            throw std::invalid_argument("teams must be different");
        }
        teams_->getTeam(teamAId);
        teams_->getTeam(teamBId);

        const std::string matchId = "match_" + std::to_string(++counter_);
        matches_[matchId] = Match{matchId,     teamAId, teamBId, venue, format,
                                  MatchStatus::SCHEDULED, "", TossDecision::BAT, 0, {}, ""};
        return matchId;
    }

    void conductToss(const std::string& matchId, const std::string& winnerTeamId,
                     TossDecision decision) {
        Match& match = getMatch(matchId);
        if (match.status != MatchStatus::SCHEDULED) {
            throw std::runtime_error("toss only before match start");
        }
        if (winnerTeamId != match.teamAId && winnerTeamId != match.teamBId) {
            throw std::invalid_argument("toss winner must be a match team");
        }
        match.tossWinnerTeamId = winnerTeamId;
        match.tossDecision = decision;
    }

    void startMatch(const std::string& matchId) {
        Match& match = getMatch(matchId);
        if (match.status != MatchStatus::SCHEDULED) {
            throw std::runtime_error("match already started or finished");
        }
        if (match.tossWinnerTeamId.empty()) {
            throw std::runtime_error("conduct toss before start");
        }

        const std::string battingFirst = resolveBattingTeam(match);
        const std::string bowlingFirst =
            battingFirst == match.teamAId ? match.teamBId : match.teamAId;

        match.innings.clear();
        match.innings.push_back(InningsScore{battingFirst, bowlingFirst, 0, 0, 0, false});
        match.currentInningsIndex = 0;
        match.status = MatchStatus::LIVE;
    }

    void startSecondInnings(const std::string& matchId) {
        Match& match = getMatch(matchId);
        if (match.status != MatchStatus::LIVE || match.innings.empty()) {
            throw std::runtime_error("cannot start second innings");
        }
        if (!match.innings[0].completed) {
            throw std::runtime_error("first innings not completed");
        }
        if (match.innings.size() >= 2) {
            throw std::runtime_error("second innings already exists");
        }

        const InningsScore& first = match.innings[0];
        match.innings.push_back(
            InningsScore{first.bowlingTeamId, first.battingTeamId, 0, 0, 0, false});
        match.currentInningsIndex = 1;
    }

    void completeMatch(const std::string& matchId, const std::string& resultSummary) {
        Match& match = getMatch(matchId);
        match.status = MatchStatus::COMPLETED;
        match.resultSummary = resultSummary;
    }

    Match& getMatch(const std::string& matchId) {
        auto it = matches_.find(matchId);
        if (it == matches_.end()) {
            throw std::invalid_argument("unknown match: " + matchId);
        }
        return it->second;
    }

    const Match& getMatch(const std::string& matchId) const {
        auto it = matches_.find(matchId);
        if (it == matches_.end()) {
            throw std::invalid_argument("unknown match: " + matchId);
        }
        return it->second;
    }

    InningsScore& currentInnings(const std::string& matchId) {
        Match& match = getMatch(matchId);
        if (match.innings.empty()) {
            throw std::runtime_error("no active innings");
        }
        return match.innings[static_cast<std::size_t>(match.currentInningsIndex)];
    }

    std::vector<std::string> listByStatus(MatchStatus status) const {
        std::vector<std::string> ids;
        for (const auto& entry : matches_) {
            if (entry.second.status == status) {
                ids.push_back(entry.first);
            }
        }
        return ids;
    }

private:
    std::string resolveBattingTeam(const Match& match) const {
        const std::string other =
            match.tossWinnerTeamId == match.teamAId ? match.teamBId : match.teamAId;
        return match.tossDecision == TossDecision::BAT ? match.tossWinnerTeamId : other;
    }

    TeamRegistryService* teams_;
    std::unordered_map<std::string, Match> matches_;
    int counter_{0};
};

}  // namespace cricbuzz_lld

#endif
