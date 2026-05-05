#ifndef CHESS_LLD_STRATEGIES_MATCHINGSTRATEGY_H
#define CHESS_LLD_STRATEGIES_MATCHINGSTRATEGY_H

#include <bits/stdc++.h>

#include "../models/User.h"

using namespace std;

namespace chess_lld {

class MatchingStrategy {
public:
    virtual ~MatchingStrategy() {}
    virtual User *findMatch(User *user, vector<User *> &waitingUsers) = 0;
};

class ScoreBasedMatching : public MatchingStrategy {
private:
    int scoreTolerance_;

public:
    explicit ScoreBasedMatching(int scoreTolerance) : scoreTolerance_(scoreTolerance) {}

    User *findMatch(User *user, vector<User *> &waitingUsers) override {
        User *bestMatch = nullptr;
        int bestScoreDiff = INT_MAX;
        for (User *waitingUser : waitingUsers) {
            if (waitingUser->getId() == user->getId()) continue;
            int scoreDiff = abs(waitingUser->getScore() - user->getScore());
            if (scoreDiff <= scoreTolerance_ && scoreDiff < bestScoreDiff) {
                bestScoreDiff = scoreDiff;
                bestMatch = waitingUser;
            }
        }
        return bestMatch;
    }
};
}

#endif
