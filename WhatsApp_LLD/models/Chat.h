#ifndef CHAT_H
#define CHAT_H

#include <string>
#include <vector>
#include "Message.h"

namespace whatsapp_lld {

class Chat {
protected:
    std::string chatId;
    std::vector<std::string> participantIds;
    std::vector<Message> messages;
    long long nextSequenceNumber;

public:
    Chat(std::string chatId, std::vector<std::string> participantIds)
        : chatId(std::move(chatId)),
          participantIds(std::move(participantIds)),
          nextSequenceNumber(1) {}

    virtual ~Chat() = default;

    const std::string& getChatId() const { return chatId; }
    const std::vector<std::string>& getParticipantIds() const { return participantIds; }
    const std::vector<Message>& getMessages() const { return messages; }

    void addMessage(Message message) {
        message.assignSequenceNumber(nextSequenceNumber++);
        messages.push_back(std::move(message));
    }
};

}  // namespace whatsapp_lld

#endif
