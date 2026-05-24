#ifndef MESSAGE_DELETION_SERVICE_H
#define MESSAGE_DELETION_SERVICE_H

#include <algorithm>
#include <ctime>
#include <stdexcept>
#include <string>
#include <vector>

#include "../enums/DeletionType.h"
#include "../models/Chat.h"
#include "../models/Message.h"

namespace whatsapp_lld {

class MessageDeletionService {
public:
    static constexpr int kDeleteForEveryoneWindowSeconds = 3600;  // WhatsApp-like ~1 hour

    void deleteMessage(Chat& chat, const std::string& messageId, const std::string& actorUserId,
                       DeletionType type, std::time_t now = std::time(nullptr)) const {
        if (!hasParticipant(chat, actorUserId)) {
            throw std::runtime_error("user is not a participant in this chat");
        }

        Message* message = findMutableMessage(chat, messageId);
        if (message == nullptr) {
            throw std::invalid_argument("message not found: " + messageId);
        }

        if (type == DeletionType::DELETE_FOR_ME) {
            message->markDeletedForMe(actorUserId);
            return;
        }

        if (message->getSenderId() != actorUserId) {
            throw std::runtime_error("only the sender can delete for everyone");
        }
        if (message->isDeletedForEveryone()) {
            throw std::runtime_error("message already deleted for everyone");
        }
        if (now - message->getSentAt() > kDeleteForEveryoneWindowSeconds) {
            throw std::runtime_error("delete-for-everyone window expired");
        }

        message->markDeletedForEveryone(now);
    }

private:
    static bool hasParticipant(const Chat& chat, const std::string& userId) {
        const std::vector<std::string>& participants = chat.getParticipantIds();
        return std::find(participants.begin(), participants.end(), userId) != participants.end();
    }

    static Message* findMutableMessage(Chat& chat, const std::string& messageId) {
        for (Message& message : chat.getMessagesMutable()) {
            if (message.getMessageId() == messageId) {
                return &message;
            }
        }
        return nullptr;
    }
};

}  // namespace whatsapp_lld

#endif
