// ============================================================================
//  models/User.h — Player model + MEDIATOR PATTERN ke interfaces (3-in-1 file!)
// ----------------------------------------------------------------------------
//  Is file me Mediator pattern ka POORA contract hai (L35 wala pattern live):
//
//    ChatMediator (interface) : "beech ka aadmi" — sendMessage/add/removeUser.
//                               Match.h isse implement karta hai.
//    Colleague (base)         : "baat karne wala" — mediator ka pointer
//                               rakhta hai, send/receive ka contract deta hai.
//    User : Colleague         : actual player — id, naam, score (1000 se
//                               shuru), aur chat methods.
//
//  MEDIATOR KA FLOW (players ek-dusre ko NAHI jaante!):
//    aditya.send(msg) ──> mediator_->sendMessage(msg, this)
//                              │ (Match decide karta hai kise dena hai)
//                              ▼
//                         rohit.receive(msg)
//  Fayda: User class me opponent ka koi pointer NAHI — Match hi routing
//  karta hai. Naya feature (group chat, spectators) aaye to sirf mediator
//  badlega, User untouched!
//
//  SCORE: matchmaking ka aadhaar (ScoreBasedMatching ±100 dekhta hai) —
//  jeet +30, haar -20, quit -50 (Match.h ke endGame/quitGame se).
// ============================================================================
#ifndef CHESS_LLD_MODELS_USER_H
#define CHESS_LLD_MODELS_USER_H

#include <bits/stdc++.h>

#include "Message.h"

using namespace std;

namespace chess_lld {
class User;

class ChatMediator {
public:
    virtual ~ChatMediator() {}
    virtual void sendMessage(Message *message, User *user) = 0;
    virtual void addUser(User *user) = 0;
    virtual void removeUser(User *user) = 0;
};

class Colleague {
protected:
    ChatMediator *mediator_;

public:
    Colleague() : mediator_(nullptr) {}
    virtual ~Colleague() {}
    virtual void send(Message *message) = 0;
    virtual void receive(Message *message) = 0;
    virtual void setMediator(ChatMediator *mediator) { mediator_ = mediator; }
};

class User : public Colleague {
private:
    string id_;
    string name_;
    int score_;

public:
    User(string id, string name) : Colleague(), id_(std::move(id)), name_(std::move(name)), score_(1000) {}

    string getId() const { return id_; }
    string getName() const { return name_; }
    int getScore() const { return score_; }
    void incrementScore(int points) { score_ += points; }
    void decrementScore(int points) { score_ -= points; }
    string toString() const { return name_ + " (Score: " + to_string(score_) + ")"; }

    void send(Message *message) override { if (mediator_ != nullptr) mediator_->sendMessage(message, this); }
    void receive(Message *message) override {
        cout << "User " << name_ << " received message from " << message->getSenderId()
             << ": " << message->getContent() << endl;
    }
};
}

#endif
