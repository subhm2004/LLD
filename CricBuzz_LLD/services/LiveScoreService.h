#ifndef CRICBUZZ_LLD_SERVICES_LIVE_SCORE_SERVICE_H
#define CRICBUZZ_LLD_SERVICES_LIVE_SCORE_SERVICE_H

#include <string>
#include <vector>

#include "../models/LiveScoreboard.h"
#include "../models/Match.h"
#include "MatchService.h"
#include "ScoringService.h"
#include "TeamRegistryService.h"

namespace cricbuzz_lld {

class LiveScoreService {
public:
    LiveScoreService(MatchService* matches, TeamRegistryService* teams, ScoringService* scoring)
        : matches_(matches), teams_(teams), scoring_(scoring) {}

    LiveScoreboard getScoreboard(const std::string& matchId) const {
        const Match& match = matches_->getMatch(matchId);
        LiveScoreboard board;
        board.matchId = matchId;
        board.status = match.status;

        if (match.innings.empty()) {
            return board;
        }

        const InningsScore& innings =
            match.innings[static_cast<std::size_t>(match.currentInningsIndex)];
        board.battingTeamName = teams_->getTeam(innings.battingTeamId).name;
        board.bowlingTeamName = teams_->getTeam(innings.bowlingTeamId).name;
        board.runs = innings.runs;
        board.wickets = innings.wickets;
        board.overs = innings.legalBalls / 6;
        board.ballsInOver = innings.legalBalls % 6;
        board.runRate = ScoringService::computeRunRate(innings.runs, innings.legalBalls);

        if (match.innings.size() == 2 && match.currentInningsIndex == 1) {
            const int target = match.innings[0].runs + 1;
            const int needed = target - innings.runs;
            board.targetInfo = "Need " + std::to_string(needed) + " to win";
        } else if (match.innings.size() >= 1 && match.currentInningsIndex == 0) {
            board.targetInfo = "1st innings";
        }

        const std::vector<BallEvent> recent = scoring_->getBallByBall(matchId, 1);
        if (!recent.empty()) {
            board.lastBallSummary = recent.back().commentary;
        }

        if (!match.resultSummary.empty()) {
            board.targetInfo = match.resultSummary;
        }
        return board;
    }

    std::vector<std::string> listLiveMatchIds() const {
        return matches_->listByStatus(MatchStatus::LIVE);
    }

private:
    MatchService* matches_;
    TeamRegistryService* teams_;
    ScoringService* scoring_;
};

}  // namespace cricbuzz_lld

#endif
