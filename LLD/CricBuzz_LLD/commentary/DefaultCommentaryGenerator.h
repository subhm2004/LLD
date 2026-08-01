#ifndef CRICBUZZ_LLD_COMMENTARY_DEFAULT_COMMENTARY_GENERATOR_H
#define CRICBUZZ_LLD_COMMENTARY_DEFAULT_COMMENTARY_GENERATOR_H

#include <string>

#include "../enums/BallOutcome.h"
#include "ICommentaryGenerator.h"

namespace cricbuzz_lld {

class DefaultCommentaryGenerator : public ICommentaryGenerator {
public:
    std::string generate(const std::string& batsmanName, const std::string& bowlerName,
                         BallOutcome outcome, int runs, bool isWicket) const override {
        if (isWicket) {
            return bowlerName + " to " + batsmanName + " — OUT! Wicket falls.";
        }
        switch (outcome) {
            case BallOutcome::SIX:
                return batsmanName + " smashes " + bowlerName + " for SIX!";
            case BallOutcome::FOUR:
                return batsmanName + " drives " + bowlerName + " for FOUR!";
            case BallOutcome::WIDE:
                return "Wide from " + bowlerName + ". Extra run.";
            case BallOutcome::NO_BALL:
                return "No ball from " + bowlerName + ". Free hit territory.";
            case BallOutcome::DOT:
                return batsmanName + " defends " + bowlerName + " — no run.";
            case BallOutcome::RUNS:
                return batsmanName + " takes " + std::to_string(runs) + " off " + bowlerName + ".";
            case BallOutcome::WICKET:
                break;
        }
        return batsmanName + " vs " + bowlerName;
    }
};

}  // namespace cricbuzz_lld

#endif
