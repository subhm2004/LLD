#ifndef PUB_SUB_SYSTEM_LLD_SERVICES_DISPATCHSERVICE_H
#define PUB_SUB_SYSTEM_LLD_SERVICES_DISPATCHSERVICE_H

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "../core/ISubscriber.h"
#include "../enums/DeliveryMode.h"
#include "../models/Message.h"
#include "SubscriptionService.h"

namespace pub_sub_lld {

class DispatchService {
public:
    explicit DispatchService(SubscriptionService &subscriptions) : subscriptions_(subscriptions) {}

    int publish(const Message &message, DeliveryMode mode = DeliveryMode::SYNC) {
        if (!subscriptions_.hasTopic(message.topic)) {
            throw std::runtime_error("Cannot publish to unknown topic: " + message.topic);
        }

        const std::vector<std::shared_ptr<ISubscriber>> &subscribers =
            subscriptions_.getSubscribers(message.topic);

        if (subscribers.empty()) {
            std::cout << "[BROKER] No subscribers on topic '" << message.topic << "' — message dropped\n";
            return 0;
        }

        int delivered = 0;
        for (const std::shared_ptr<ISubscriber> &subscriber : subscribers) {
            if (mode == DeliveryMode::SYNC) {
                subscriber->onMessage(message);
            } else {
                dispatchAsync(subscriber, message);
            }
            ++delivered;
        }
        return delivered;
    }

private:
    SubscriptionService &subscriptions_;

    void dispatchAsync(const std::shared_ptr<ISubscriber> &subscriber, const Message &message) const {
        // Demo: synchronous call — real system would push to queue + worker pool
        std::cout << "[BROKER][ASYNC-queue] enqueue -> " << subscriber->getId() << "\n";
        subscriber->onMessage(message);
    }
};

} // namespace pub_sub_lld

#endif // PUB_SUB_SYSTEM_LLD_SERVICES_DISPATCHSERVICE_H
