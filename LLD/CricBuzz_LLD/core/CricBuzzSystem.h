#ifndef CRICBUZZ_LLD_CORE_CRICBUZZ_SYSTEM_H
#define CRICBUZZ_LLD_CORE_CRICBUZZ_SYSTEM_H

#include <string>
#include <vector>

#include "../commentary/DefaultCommentaryGenerator.h"
#include "../enums/BallOutcome.h"
#include "../enums/MatchFormat.h"
#include "../enums/TossDecision.h"
#include "../models/BallEvent.h"
#include "../models/LiveScoreboard.h"
#include "../models/Match.h"
#include "../models/Team.h"
#include "../services/LiveScoreService.h"
#include "../services/MatchService.h"
#include "../services/ScoringService.h"
#include "../services/TeamRegistryService.h"

namespace cricbuzz_lld {

// Facade — CricBuzz-style live cricket scoring API
class CricBuzzSystem {
public:
    CricBuzzSystem()
        : matches_(&teams_),
          scoring_(&matches_, &teams_, &commentary_),
          liveScore_(&matches_, &teams_, &scoring_) {}

    std::string registerTeam(const std::string& name, const std::string& shortCode) {
        return teams_.registerTeam(name, shortCode);
    }

    std::string registerPlayer(const std::string& teamId, const std::string& name) {
        return teams_.registerPlayer(teamId, name);
    }

    std::string scheduleMatch(const std::string& teamAId, const std::string& teamBId,
                              const std::string& venue, MatchFormat format) {
        return matches_.scheduleMatch(teamAId, teamBId, venue, format);
    }

    void conductToss(const std::string& matchId, const std::string& winnerTeamId,
                     TossDecision decision) {
        matches_.conductToss(matchId, winnerTeamId, decision);
    }

    void startMatch(const std::string& matchId) { matches_.startMatch(matchId); }

    BallEvent recordBall(const std::string& matchId, const std::string& batsmanId,
                         const std::string& bowlerId, BallOutcome outcome, int runs = 0,
                         const std::string& wicketType = "") {
        return scoring_.recordBall(matchId, batsmanId, bowlerId, outcome, runs, wicketType);
    }

    void endInnings(const std::string& matchId) {
        InningsScore& innings = matches_.currentInnings(matchId);
        innings.completed = true;
    }

    void startSecondInnings(const std::string& matchId) { matches_.startSecondInnings(matchId); }

    void endMatch(const std::string& matchId, const std::string& resultSummary) {
        matches_.completeMatch(matchId, resultSummary);
    }

    LiveScoreboard getLiveScore(const std::string& matchId) const {
        return liveScore_.getScoreboard(matchId);
    }

    std::vector<BallEvent> getCommentaryFeed(const std::string& matchId, int lastN = 10) const {
        return scoring_.getBallByBall(matchId, lastN);
    }

    std::vector<std::string> listLiveMatches() const { return liveScore_.listLiveMatchIds(); }

    Match getMatch(const std::string& matchId) const { return matches_.getMatch(matchId); }

    const Team& getTeam(const std::string& teamId) const { return teams_.getTeam(teamId); }

private:
    TeamRegistryService teams_;
    DefaultCommentaryGenerator commentary_;
    MatchService matches_;
    ScoringService scoring_;
    LiveScoreService liveScore_;
};

}  // namespace cricbuzz_lld

#endif
