#ifndef CHAT_SERVICE_H
#define CHAT_SERVICE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/DeletionType.h"
#include "../models/Chat.h"
#include "../models/Group.h"
#include "../models/Message.h"
#include "EncryptionService.h"
#include "MessageDeletionService.h"

namespace whatsapp_lld {

class ChatService {
public:
    ChatService() : encryptionService_(&defaultEncryptionService_) {}

    void setEncryptionService(const EncryptionService* service) {
        if (service != nullptr) {
            encryptionService_ = service;
        }
    }

    void createDirectChat(const std::string& chatId, const std::vector<std::string>& participants) {
        directChats_.emplace(chatId, Chat(chatId, participants));
    }

    void createGroup(const std::string& chatId, const std::string& groupName, const std::string& adminId,
                     const std::vector<std::string>& participants) {
        groups_.emplace(chatId, Group(chatId, groupName, adminId, participants));
    }

    void sendDirectMessage(const std::string& chatId, const Message& message) {
        directChats_.at(chatId).addMessage(encryptForStore(chatId, message));
    }

    void sendGroupMessage(const std::string& chatId, const Message& message) {
        groups_.at(chatId).addMessage(encryptForStore(chatId, message));
    }

    void deleteDirectMessage(const std::string& chatId, const std::string& messageId,
                             const std::string& userId, DeletionType type, std::time_t now) {
        deletionService_.deleteMessage(directChats_.at(chatId), messageId, userId, type, now);
    }

    void deleteGroupMessage(const std::string& chatId, const std::string& messageId,
                            const std::string& userId, DeletionType type, std::time_t now) {
        deletionService_.deleteMessage(groups_.at(chatId), messageId, userId, type, now);
    }

    const Chat& getDirectChat(const std::string& chatId) const { return directChats_.at(chatId); }

    const Group& getGroup(const std::string& chatId) const { return groups_.at(chatId); }

    std::vector<Message> getDirectMessagesForUser(const std::string& chatId,
                                                const std::string& userId) const {
        return decryptVisibleMessages(chatId, directChats_.at(chatId).getVisibleMessagesForUser(userId));
    }

    std::vector<Message> getGroupMessagesForUser(const std::string& chatId,
                                                 const std::string& userId) const {
        return decryptVisibleMessages(chatId, groups_.at(chatId).getVisibleMessagesForUser(userId));
    }

private:
    Message encryptForStore(const std::string& chatId, const Message& message) const {
        Message stored = message;
        stored.setContent(encryptionService_->encrypt(message.getContent(), chatId));
        return stored;
    }

    std::vector<Message> decryptVisibleMessages(const std::string& chatId,
                                                std::vector<Message> messages) const {
        for (Message& message : messages) {
            if (message.isDeletedForEveryone()) {
                continue;
            }
            message.setContent(encryptionService_->decrypt(message.getContent(), chatId));
        }
        return messages;
    }

    std::unordered_map<std::string, Chat> directChats_;
    std::unordered_map<std::string, Group> groups_;
    NoOpEncryptionService defaultEncryptionService_;
    const EncryptionService* encryptionService_;
    MessageDeletionService deletionService_;
};

}  // namespace whatsapp_lld

#endif
