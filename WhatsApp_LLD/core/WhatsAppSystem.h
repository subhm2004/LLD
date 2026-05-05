#ifndef WHATSAPP_SYSTEM_H
#define WHATSAPP_SYSTEM_H

#include <string>
#include <ctime>
#include <unordered_map>
#include "../models/User.h"
#include "../services/ChatService.h"
#include "../services/EncryptionService.h"
#include "../services/NotificationService.h"

namespace whatsapp_lld {

class WhatsAppSystem {
private:
    std::unordered_map<std::string, User> users;
    ChatService chatService;
    NotificationService notificationService;

public:
    void registerUser(const std::string& userId, const std::string& name, const std::string& phone) {
        users.emplace(userId, User(userId, name, phone));
    }

    void createDirectChat(const std::string& chatId, const std::vector<std::string>& participants) {
        chatService.createDirectChat(chatId, participants);
    }

    void setUserOnline(const std::string& userId) {
        users.at(userId).setOnline(true);
    }

    void setUserOffline(const std::string& userId) {
        users.at(userId).setOnline(false);
    }

    bool isUserOnline(const std::string& userId) const {
        return users.at(userId).isOnline();
    }

    std::time_t getUserLastSeen(const std::string& userId) const {
        return users.at(userId).getLastSeenAt();
    }

    void createGroup(const std::string& chatId, const std::string& groupName, const std::string& adminId,
                     const std::vector<std::string>& participants) {
        chatService.createGroup(chatId, groupName, adminId, participants);
    }

    void configureEncryption(const EncryptionService* encryptionService) {
        chatService.setEncryptionService(encryptionService);
    }

    void sendDirectMessage(const std::string& chatId, const Message& message, const std::string& receiverId) {
        chatService.sendDirectMessage(chatId, message);
        notificationService.notifyUser(receiverId, "New direct message");
    }

    void sendGroupMessage(const std::string& chatId, const Message& message) {
        chatService.sendGroupMessage(chatId, message);
    }

    const Chat& getDirectChat(const std::string& chatId) const {
        return chatService.getDirectChat(chatId);
    }

    const Group& getGroup(const std::string& chatId) const {
        return chatService.getGroup(chatId);
    }
};

}  // namespace whatsapp_lld

#endif
