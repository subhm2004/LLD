#ifndef MESSAGE_H
#define MESSAGE_H

#include <ctime>
#include <string>
#include <unordered_set>

#include "../enums/MessageStatus.h"
#include "../enums/MessageType.h"

namespace whatsapp_lld {

class Message {
public:
    static constexpr const char* kDeletedForEveryonePlaceholder = "This message was deleted";

    Message(std::string messageId, std::string senderId, std::string content, MessageType messageType)
        : messageId_(std::move(messageId)),
          senderId_(std::move(senderId)),
          content_(std::move(content)),
          sequenceNumber_(-1),
          messageType_(messageType),
          status_(MessageStatus::SENT),
          sentAt_(0) {}

    const std::string& getMessageId() const { return messageId_; }
    const std::string& getSenderId() const { return senderId_; }
    const std::string& getContent() const { return content_; }
    long long getSequenceNumber() const { return sequenceNumber_; }
    MessageType getMessageType() const { return messageType_; }
    MessageStatus getStatus() const { return status_; }
    std::time_t getSentAt() const { return sentAt_; }
    bool isDeletedForEveryone() const { return deletedForEveryone_; }
    bool isDeletedForMeBy(const std::string& userId) const {
        return deletedForUserIds_.count(userId) > 0;
    }

    // Hidden from this user's chat timeline (delete-for-me OR global delete they still see tombstone)
    bool isHiddenForUser(const std::string& userId) const {
        return deletedForUserIds_.count(userId) > 0;
    }

    std::string getDisplayContent(const std::string& viewerId) const {
        if (isHiddenForUser(viewerId)) {
            return "";
        }
        if (deletedForEveryone_) {
            return kDeletedForEveryonePlaceholder;
        }
        return content_;
    }

    void setContent(const std::string& newContent) { content_ = newContent; }
    void assignSequenceNumber(long long seq) { sequenceNumber_ = seq; }
    void stampSentAt(std::time_t sentAt) { sentAt_ = sentAt; }
    void updateStatus(MessageStatus newStatus) { status_ = newStatus; }

    void markDeletedForMe(const std::string& userId) { deletedForUserIds_.insert(userId); }

    void markDeletedForEveryone(std::time_t deletedAt) {
        deletedForEveryone_ = true;
        deletedForEveryoneAt_ = deletedAt;
        content_.clear();
    }

private:
    std::string messageId_;
    std::string senderId_;
    std::string content_;
    long long sequenceNumber_;
    MessageType messageType_;
    MessageStatus status_;
    std::time_t sentAt_;

    bool deletedForEveryone_{false};
    std::time_t deletedForEveryoneAt_{0};
    std::unordered_set<std::string> deletedForUserIds_;
};

}  // namespace whatsapp_lld

#endif
