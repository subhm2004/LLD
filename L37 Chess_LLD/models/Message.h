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
