#ifndef WHATSAPP_SYSTEM_H
#define WHATSAPP_SYSTEM_H

#include <ctime>
#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/DeletionType.h"
#include "../models/Chat.h"
#include "../models/Group.h"
#include "../models/Message.h"
#include "../models/User.h"
#include "../services/ChatService.h"
#include "../services/EncryptionService.h"
#include "../services/NotificationService.h"

namespace whatsapp_lld {

class WhatsAppSystem {
public:
    void registerUser(const std::string& userId, const std::string& name, const std::string& phone) {
        users_.emplace(userId, User(userId, name, phone));
    }

    void createDirectChat(const std::string& chatId, const std::vector<std::string>& participants) {
        chatService_.createDirectChat(chatId, participants);
    }

    void setUserOnline(const std::string& userId) { users_.at(userId).setOnline(true); }

    void setUserOffline(const std::string& userId) { users_.at(userId).setOnline(false); }

    bool isUserOnline(const std::string& userId) const { return users_.at(userId).isOnline(); }

    std::time_t getUserLastSeen(const std::string& userId) const {
        return users_.at(userId).getLastSeenAt();
    }

    void createGroup(const std::string& chatId, const std::string& groupName, const std::string& adminId,
                     const std::vector<std::string>& participants) {
        chatService_.createGroup(chatId, groupName, adminId, participants);
    }

    void configureEncryption(const EncryptionService* encryptionService) {
        chatService_.setEncryptionService(encryptionService);
    }

    void sendDirectMessage(const std::string& chatId, const Message& message,
                           const std::string& receiverId) {
        chatService_.sendDirectMessage(chatId, message);
        notificationService_.notifyUser(receiverId, "New direct message");
    }

    void sendGroupMessage(const std::string& chatId, const Message& message) {
        chatService_.sendGroupMessage(chatId, message);
    }

    void deleteDirectMessage(const std::string& chatId, const std::string& messageId,
                             const std::string& userId, DeletionType type,
                             std::time_t now = std::time(nullptr)) {
        chatService_.deleteDirectMessage(chatId, messageId, userId, type, now);
    }

    void deleteGroupMessage(const std::string& chatId, const std::string& messageId,
                            const std::string& userId, DeletionType type,
                            std::time_t now = std::time(nullptr)) {
        chatService_.deleteGroupMessage(chatId, messageId, userId, type, now);
    }

    std::vector<Message> getDirectMessagesForUser(const std::string& chatId,
                                                  const std::string& userId) const {
        return chatService_.getDirectMessagesForUser(chatId, userId);
    }

    std::vector<Message> getGroupMessagesForUser(const std::string& chatId,
                                                 const std::string& userId) const {
        return chatService_.getGroupMessagesForUser(chatId, userId);
    }

    const Chat& getDirectChat(const std::string& chatId) const {
        return chatService_.getDirectChat(chatId);
    }

    const Group& getGroup(const std::string& chatId) const { return chatService_.getGroup(chatId); }

private:
    std::unordered_map<std::string, User> users_;
    ChatService chatService_;
    NotificationService notificationService_;
};

}  // namespace whatsapp_lld

#endif
