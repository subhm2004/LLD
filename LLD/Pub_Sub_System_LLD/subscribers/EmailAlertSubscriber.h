#ifndef PUB_SUB_SYSTEM_LLD_SUBSCRIBERS_EMAILALERTSUBSCRIBER_H
#define PUB_SUB_SYSTEM_LLD_SUBSCRIBERS_EMAILALERTSUBSCRIBER_H

#include <iostream>
#include <string>

#include "../core/ISubscriber.h"

namespace pub_sub_lld {

class EmailAlertSubscriber : public ISubscriber {
public:
    EmailAlertSubscriber(std::string id, std::string email)
        : id_(std::move(id)), email_(std::move(email)) {}

    std::string getId() const override { return id_; }

    void onMessage(const Message &message) override {
        std::cout << "[EMAIL][" << id_ << " -> " << email_ << "] Alert for topic '" << message.topic
                  << "': " << message.payload << "\n";
    }

private:
    std::string id_;
    std::string email_;
};

} // namespace pub_sub_lld

#endif // PUB_SUB_SYSTEM_LLD_SUBSCRIBERS_EMAILALERTSUBSCRIBER_H
