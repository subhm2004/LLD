// models/Recipient.h — Notification ka receiver: naam + contact details
// (email/phone) + preferred channel. Engine isse decide karta hai kahan bhejna.
#ifndef RECIPIENT_H
#define RECIPIENT_H

#include <string>
#include <utility>
#include "../enums/ChannelType.h"

namespace notification_lld {

class Recipient {
private:
    std::string id_;
    std::string name_;
    std::string address_;
    ChannelType preferredChannel_;

public:
    Recipient(std::string id, std::string name, std::string address, ChannelType preferredChannel)
        : id_(std::move(id)),
          name_(std::move(name)),
          address_(std::move(address)),
          preferredChannel_(preferredChannel) {}

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }
    const std::string& getAddress() const { return address_; }
    ChannelType getPreferredChannel() const { return preferredChannel_; }
};

}  // namespace notification_lld

#endif
