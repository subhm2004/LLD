// models/Message.h — Ek chat message: sender, text, timestamp. ChatRoom inhi ki
// list rakhta hai.
#ifndef TINDER_LLD_MODELS_MESSAGE_H
#define TINDER_LLD_MODELS_MESSAGE_H

#include <bits/stdc++.h>

using namespace std;

namespace tinder_lld {

class Message {
public:
    Message(string senderId, string content)
        : senderId_(std::move(senderId)), content_(std::move(content)), timestamp_(time(nullptr)), read_(false) {}

    const string &getSenderId() const { return senderId_; }
    const string &getContent() const { return content_; }
    long long getTimestamp() const { return timestamp_; }
    bool isRead() const { return read_; }
    void markRead() { read_ = true; }

private:
    string senderId_;
    string content_;
    long long timestamp_;
    bool read_;
};

} // namespace tinder_lld

#endif // TINDER_LLD_MODELS_MESSAGE_H
