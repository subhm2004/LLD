// ============================================================================
//  strategies/MatchingStrategy.h — STRATEGY: matchmaking ka algorithm 🤝
// ----------------------------------------------------------------------------
//  "Kis user ko kis se ladaya jaye?" — ye decision swap-able strategy hai
//  (L8 wala pattern). GameManager sirf MatchingStrategy* jaanta hai.
//
//  ScoreBasedMatching ka algorithm (chess.com ka simplified version):
//    - waiting users me se wo dhundo jiska score mere score ke SABSE PAAS
//      ho (best match), PAR tolerance ke andar (yahan ±100)
//    - khud se match na ho jaye — id check!
//    - koi tolerance me nahi mila? -> nullptr (caller queue me daal dega)
//  Example: mera score 1000, waiting me 1080 aur 950 -> 950 milega
//  (diff 50 < 80). Waiting me sirf 1150? -> nullptr (diff 150 > 100).
//
//  Naya algorithm chahiye (region-based? time-control? random?) ->
//  nayi subclass banao, GameManager me inject karo — zero edit! (OCP)
// ============================================================================
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
