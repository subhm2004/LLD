#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include "../enums/MessageStatus.h"
#include "../enums/MessageType.h"

namespace whatsapp_lld {

class Message {
private:
    std::string messageId;
    std::string senderId;
    std::string content;
    long long sequenceNumber;
    MessageType messageType;
    MessageStatus status;

public:
    Message(std::string messageId, std::string senderId, std::string content, MessageType messageType)
        : messageId(std::move(messageId)),
          senderId(std::move(senderId)),
          content(std::move(content)),
          sequenceNumber(-1),
          messageType(messageType),
          status(MessageStatus::SENT) {}

    const std::string& getMessageId() const { return messageId; }
    const std::string& getSenderId() const { return senderId; }
    const std::string& getContent() const { return content; }
    long long getSequenceNumber() const { return sequenceNumber; }
    MessageType getMessageType() const { return messageType; }
    MessageStatus getStatus() const { return status; }

    void setContent(const std::string& newContent) { content = newContent; }
    void assignSequenceNumber(long long seq) { sequenceNumber = seq; }
    void updateStatus(MessageStatus newStatus) { status = newStatus; }
};

}  // namespace whatsapp_lld

#endif
