#ifndef CRICBUZZ_LLD_SERVICES_SCORING_SERVICE_H
#define CRICBUZZ_LLD_SERVICES_SCORING_SERVICE_H

#include <cmath>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../commentary/ICommentaryGenerator.h"
#include "../enums/BallOutcome.h"
#include "../enums/MatchFormat.h"
#include "../enums/MatchStatus.h"
#include "../models/BallEvent.h"
#include "../models/InningsScore.h"
#include "../models/Match.h"
#include "MatchService.h"
#include "TeamRegistryService.h"

namespace cricbuzz_lld {

class ScoringService {
public:
    ScoringService(MatchService* matches, TeamRegistryService* teams, ICommentaryGenerator* commentary)
        : matches_(matches), teams_(teams), commentary_(commentary) {}

    BallEvent recordBall(const std::string& matchId, const std::string& batsmanId,
                         const std::string& bowlerId, BallOutcome outcome, int runs = 0,
                         const std::string& wicketType = "") {
        Match& match = matches_->getMatch(matchId);
        if (match.status != MatchStatus::LIVE) {
            throw std::runtime_error("match is not live");
        }

        InningsScore& innings = matches_->currentInnings(matchId);
        validatePlayers(batsmanId, bowlerId, innings);

        const bool isExtra = outcome == BallOutcome::WIDE || outcome == BallOutcome::NO_BALL;
        const bool isWicket = outcome == BallOutcome::WICKET;
        int runsOnBall = resolveRuns(outcome, runs);

        if (isWicket) {
            if (innings.wickets >= 10) {
                throw std::runtime_error("innings already all out");
            }
            ++innings.wickets;
        }

        innings.runs += runsOnBall;

        int overNumber = 0;
        int ballInOver = 0;
        if (!isExtra) {
            overNumber = innings.legalBalls / 6;
            ballInOver = (innings.legalBalls % 6) + 1;
            ++innings.legalBalls;
        } else {
            overNumber = innings.legalBalls / 6;
            ballInOver = innings.legalBalls % 6;
            if (ballInOver == 0) {
                ballInOver = 6;
            }
        }

        const Player& batsman = teams_->getPlayer(batsmanId);
        const Player& bowler = teams_->getPlayer(bowlerId);
        const std::string text =
            commentary_->generate(batsman.name, bowler.name, outcome, runsOnBall, isWicket);

        const std::string eventId = "ball_" + std::to_string(++eventCounter_);
        BallEvent event{eventId,
                        matchId,
                        match.currentInningsIndex,
                        overNumber,
                        ballInOver,
                        batsmanId,
                        bowlerId,
                        outcome,
                        runsOnBall,
                        isWicket,
                        wicketType,
                        text};
        feed_[matchId].push_back(event);

        if (isInningsComplete(match, innings)) {
            innings.completed = true;
        }
        return event;
    }

    std::vector<BallEvent> getBallByBall(const std::string& matchId, int lastN = 0) const {
        auto it = feed_.find(matchId);
        if (it == feed_.end()) {
            return {};
        }
        if (lastN <= 0 || static_cast<int>(it->second.size()) <= lastN) {
            return it->second;
        }
        return std::vector<BallEvent>(it->second.end() - lastN, it->second.end());
    }

    static std::string formatOvers(int legalBalls) {
        const int overs = legalBalls / 6;
        const int balls = legalBalls % 6;
        return std::to_string(overs) + "." + std::to_string(balls);
    }

    static std::string computeRunRate(int runs, int legalBalls) {
        if (legalBalls == 0) {
            return "0.00";
        }
        const double rr = (static_cast<double>(runs) / legalBalls) * 6.0;
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << rr;
        return oss.str();
    }

private:
    static int resolveRuns(BallOutcome outcome, int runs) {
        switch (outcome) {
            case BallOutcome::DOT:
                return 0;
            case BallOutcome::FOUR:
                return 4;
            case BallOutcome::SIX:
                return 6;
            case BallOutcome::WIDE:
            case BallOutcome::NO_BALL:
                return 1 + runs;
            case BallOutcome::WICKET:
                return runs;
            case BallOutcome::RUNS:
                if (runs < 0 || runs > 3) {
                    throw std::invalid_argument("RUNS outcome needs 0-3 (use FOUR/SIX for boundaries)");
                }
                return runs;
        }
        return 0;
    }

    void validatePlayers(const std::string& batsmanId, const std::string& bowlerId,
                         const InningsScore& innings) const {
        const Player& batsman = teams_->getPlayer(batsmanId);
        const Player& bowler = teams_->getPlayer(bowlerId);
        if (batsman.teamId != innings.battingTeamId) {
            throw std::invalid_argument("batsman not in batting team");
        }
        if (bowler.teamId != innings.bowlingTeamId) {
            throw std::invalid_argument("bowler not in bowling team");
        }
    }

    bool isInningsComplete(const Match& match, const InningsScore& innings) const {
        if (innings.wickets >= 10) {
            return true;
        }
        const int maxOvers = maxOversForFormat(match.format);
        return innings.legalBalls >= maxOvers * 6;
    }

    MatchService* matches_;
    TeamRegistryService* teams_;
    ICommentaryGenerator* commentary_;
    std::unordered_map<std::string, std::vector<BallEvent>> feed_;
    int eventCounter_{0};
};

}  // namespace cricbuzz_lld

#endif
