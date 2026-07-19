// ============================================================================
//  models/Message.h — Chat ka ek message (sender + text + timestamp)
// ----------------------------------------------------------------------------
//  Simple immutable data holder — banne ke baad badalta nahi (koi setter
//  nahi). timestamp_ constructor me time(0) se auto-set hota hai.
//  Flow me role: User.send(new Message(...)) -> Match (Mediator) isse
//  chatHistory_ me store karta hai + opponent ko receive() karwata hai.
//  Ownership note: Message heap pe banta hai, Match ka destructor
//  chatHistory_ ke saare messages delete karta hai.
// ============================================================================
#ifndef CHESS_LLD_MODELS_MESSAGE_H
#define CHESS_LLD_MODELS_MESSAGE_H

#include <bits/stdc++.h>
using namespace std;

namespace chess_lld {
class Message {
private:
    string senderId_;
    string content_;
    time_t timestamp_;

public:
    Message(string senderId, string content)
        : senderId_(std::move(senderId)), content_(std::move(content)), timestamp_(time(0)) {}

    string getSenderId() const { return senderId_; }
    string getContent() const { return content_; }
    time_t getTimestamp() const { return timestamp_; }
};
}

#endif
