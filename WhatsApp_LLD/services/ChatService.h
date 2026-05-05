#ifndef CHAT_SERVICE_H
#define CHAT_SERVICE_H

#include <string>
#include <unordered_map>
#include "../models/Chat.h"
#include "../models/Group.h"
#include "EncryptionService.h"

namespace whatsapp_lld {

class ChatService {
private:
    std::unordered_map<std::string, Chat> directChats;
    std::unordered_map<std::string, Group> groups;
    NoOpEncryptionService defaultEncryptionService;
    const EncryptionService* encryptionService;

    Message getEncryptedMessage(const Message& message) const {
        Message encryptedMessage = message;
        encryptedMessage.setContent(encryptionService->encrypt(message.getContent()));
        return encryptedMessage;
    }

public:
    ChatService() : encryptionService(&defaultEncryptionService) {}

    void setEncryptionService(const EncryptionService* service) {
        if (service != nullptr) {
            encryptionService = service;
        }
    }

    void createDirectChat(const std::string& chatId, const std::vector<std::string>& participants) {
        directChats.emplace(chatId, Chat(chatId, participants));
    }

    void createGroup(const std::string& chatId, const std::string& groupName, const std::string& adminId,
                     const std::vector<std::string>& participants) {
        groups.emplace(chatId, Group(chatId, groupName, adminId, participants));
    }

    void sendDirectMessage(const std::string& chatId, const Message& message) {
        directChats.at(chatId).addMessage(getEncryptedMessage(message));
    }

    void sendGroupMessage(const std::string& chatId, const Message& message) {
        groups.at(chatId).addMessage(getEncryptedMessage(message));
    }

    const Chat& getDirectChat(const std::string& chatId) const {
        return directChats.at(chatId);
    }

    const Group& getGroup(const std::string& chatId) const {
        return groups.at(chatId);
    }
};

}  // namespace whatsapp_lld

#endif
