#ifndef CHAT_H
#define CHAT_H

#include <algorithm>
#include <ctime>
#include <string>
#include <vector>

#include "Message.h"

namespace whatsapp_lld {

class Chat {
public:
    Chat(std::string chatId, std::vector<std::string> participantIds)
        : chatId_(std::move(chatId)),
          participantIds_(std::move(participantIds)),
          nextSequenceNumber_(1) {}

    virtual ~Chat() = default;

    const std::string& getChatId() const { return chatId_; }
    const std::vector<std::string>& getParticipantIds() const { return participantIds_; }
    const std::vector<Message>& getMessages() const { return messages_; }
    std::vector<Message>& getMessagesMutable() { return messages_; }

    bool isParticipant(const std::string& userId) const { return hasParticipant(userId); }

    bool hasParticipant(const std::string& userId) const {
        return std::find(participantIds_.begin(), participantIds_.end(), userId) !=
               participantIds_.end();
    }

    void addMessage(Message message) {
        message.assignSequenceNumber(nextSequenceNumber_++);
        message.stampSentAt(std::time(nullptr));
        messages_.push_back(std::move(message));
    }

    Message* findMessageById(const std::string& messageId) {
        for (Message& message : messages_) {
            if (message.getMessageId() == messageId) {
                return &message;
            }
        }
        return nullptr;
    }

    Message* findMessage(const std::string& messageId) { return findMessageById(messageId); }

    const Message* findMessageById(const std::string& messageId) const {
        for (const Message& message : messages_) {
            if (message.getMessageId() == messageId) {
                return &message;
            }
        }
        return nullptr;
    }

    const Message* findMessage(const std::string& messageId) const {
        return findMessageById(messageId);
    }

    // Per-user timeline — respects delete-for-me; shows tombstone for delete-for-everyone
    std::vector<Message> getVisibleMessagesForUser(const std::string& userId) const {
        std::vector<Message> visible;
        for (const Message& message : messages_) {
            if (message.isHiddenForUser(userId)) {
                continue;
            }
            visible.push_back(message);
        }
        return visible;
    }

protected:
    std::string chatId_;
    std::vector<std::string> participantIds_;
    std::vector<Message> messages_;
    long long nextSequenceNumber_;
};

}  // namespace whatsapp_lld

#endif
