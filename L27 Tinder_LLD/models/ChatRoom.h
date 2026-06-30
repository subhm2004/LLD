// models/ChatRoom.h — Match hone ke baad do users ke beech chat: messages ki
// list + send/read. Sirf matched users ke beech banta hai.
#ifndef TINDER_LLD_MODELS_CHATROOM_H
#define TINDER_LLD_MODELS_CHATROOM_H

#include <bits/stdc++.h>

#include "Message.h"

using namespace std;

namespace tinder_lld {

class ChatRoom {
public:
    ChatRoom(string roomId, string userA, string userB)
        : roomId_(std::move(roomId)), userA_(std::move(userA)), userB_(std::move(userB)) {}

    void addMessage(const string &senderId, const string &content) { messages_.push_back(Message(senderId, content)); }
    void markMessagesReadFor(const string &readerId) {
        for (Message &message : messages_) {
            if (message.getSenderId() != readerId) {
                message.markRead();
            }
        }
    }

    const vector<Message> &getMessages() const { return messages_; }
    bool hasUser(const string &userId) const { return userA_ == userId || userB_ == userId; }
    const string &getRoomId() const { return roomId_; }

private:
    string roomId_;
    string userA_;
    string userB_;
    vector<Message> messages_;
};

} // namespace tinder_lld

#endif // TINDER_LLD_MODELS_CHATROOM_H
