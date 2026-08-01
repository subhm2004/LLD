#ifndef PUB_SUB_SYSTEM_LLD_SUBSCRIBERS_LOGGINGSUBSCRIBER_H
#define PUB_SUB_SYSTEM_LLD_SUBSCRIBERS_LOGGINGSUBSCRIBER_H

#include <iostream>
#include <string>

#include "../core/ISubscriber.h"

namespace pub_sub_lld {

class LoggingSubscriber : public ISubscriber {
public:
    explicit LoggingSubscriber(std::string id) : id_(std::move(id)) {}

    std::string getId() const override { return id_; }

    void onMessage(const Message &message) override {
        std::cout << "[LOG][" << id_ << "] topic=" << message.topic << " payload=" << message.payload
                  << " msgId=" << message.messageId << "\n";
    }

private:
    std::string id_;
};

} // namespace pub_sub_lld

#endif // PUB_SUB_SYSTEM_LLD_SUBSCRIBERS_LOGGINGSUBSCRIBER_H
