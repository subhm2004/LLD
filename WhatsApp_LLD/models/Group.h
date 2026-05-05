#ifndef GROUP_H
#define GROUP_H

#include <string>
#include <vector>
#include "Chat.h"

namespace whatsapp_lld {

class Group : public Chat {
private:
    std::string groupName;
    std::string adminId;

public:
    Group(std::string chatId, std::string groupName, std::string adminId, std::vector<std::string> participants)
        : Chat(std::move(chatId), std::move(participants)),
          groupName(std::move(groupName)),
          adminId(std::move(adminId)) {}

    const std::string& getGroupName() const { return groupName; }
    const std::string& getAdminId() const { return adminId; }
};

}  // namespace whatsapp_lld

#endif
