#ifndef CHESS_LLD_CORE_GAMEMANAGER_H
#define CHESS_LLD_CORE_GAMEMANAGER_H

#include <bits/stdc++.h>

#include "../strategies/MatchingStrategy.h"
#include "Match.h"

using namespace std;

namespace chess_lld {

class GameManager {
private:
    static GameManager *instance_;
    map<string, Match *> activeMatches_;
    vector<User *> waitingUsers_;
    MatchingStrategy *matchingStrategy_;
    int matchCounter_;

    GameManager() : matchingStrategy_(new ScoreBasedMatching(100)), matchCounter_(0) {}

public:
    static GameManager *getInstance() {
        if (instance_ == nullptr) instance_ = new GameManager();
        return instance_;
    }

    ~GameManager() {
        delete matchingStrategy_;
        for (auto &entry : activeMatches_) delete entry.second;
    }

    void requestMatch(User *user) {
        cout << user->getName() << " is looking for a match..." << endl;
        User *opponent = matchingStrategy_->findMatch(user, waitingUsers_);
        if (opponent == nullptr) {
            waitingUsers_.push_back(user);
            cout << user->getName() << " added to waiting list." << endl;
            return;
        }
        waitingUsers_.erase(remove(waitingUsers_.begin(), waitingUsers_.end(), opponent), waitingUsers_.end());
        string matchId = "MATCH_" + to_string(++matchCounter_);
        Match *match = new Match(matchId, user, opponent);
        activeMatches_[matchId] = match;
        cout << "Match found! " << user->getName() << " vs " << opponent->getName() << endl;
        match->getBoard()->display();
    }

    void displayActiveMatches() {
        cout << "\n=== Active Matches ===" << endl;
        for (auto &entry : activeMatches_) {
            Match *match = entry.second;
            cout << "Match " << match->getMatchId() << ": "
                 << match->getWhitePlayer()->getName() << " vs "
                 << match->getBlackPlayer()->getName() << endl;
        }
        cout << "Total active matches: " << activeMatches_.size() << endl;
        cout << "Users waiting: " << waitingUsers_.size() << endl;
    }
};

GameManager *GameManager::instance_ = nullptr;
}

#endif
