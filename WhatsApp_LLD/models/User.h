#ifndef USER_H
#define USER_H

#include <ctime>
#include <string>

namespace whatsapp_lld {

class User {
private:
    std::string userId;
    std::string name;
    std::string phone;
    bool online;
    std::time_t lastSeenAt;

public:
    User(std::string userId, std::string name, std::string phone)
        : userId(std::move(userId)),
          name(std::move(name)),
          phone(std::move(phone)),
          online(false),
          lastSeenAt(std::time(nullptr)) {}

    const std::string& getUserId() const { return userId; }
    const std::string& getName() const { return name; }
    const std::string& getPhone() const { return phone; }
    bool isOnline() const { return online; }
    std::time_t getLastSeenAt() const { return lastSeenAt; }

    void setOnline(bool isOnlineNow) {
        online = isOnlineNow;
        if (!online) {
            lastSeenAt = std::time(nullptr);
        }
    }
};

}  // namespace whatsapp_lld

#endif
