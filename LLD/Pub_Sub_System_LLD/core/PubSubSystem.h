#ifndef PUB_SUB_SYSTEM_LLD_CORE_PUBSUBSYSTEM_H
#define PUB_SUB_SYSTEM_LLD_CORE_PUBSUBSYSTEM_H

#include <iostream>
#include <memory>
#include <string>

#include "../services/DispatchService.h"
#include "../services/SubscriptionService.h"
#include "ISubscriber.h"
#include "Publisher.h"

namespace pub_sub_lld {

class PubSubSystem {
public:
    PubSubSystem() : dispatchService_(subscriptionService_), publisher_(dispatchService_, messageCounter_) {}

    void registerSubscriber(std::shared_ptr<ISubscriber> subscriber) {
        subscriptionService_.registerSubscriber(std::move(subscriber));
    }

    void createTopic(const std::string &topic) { subscriptionService_.createTopic(topic); }

    void subscribe(const std::string &topic, const std::string &subscriberId) {
        subscriptionService_.subscribe(topic, subscriberId);
        std::cout << "Subscribed " << subscriberId << " -> " << topic << "\n";
    }

    void unsubscribe(const std::string &topic, const std::string &subscriberId) {
        subscriptionService_.unsubscribe(topic, subscriberId);
        std::cout << "Unsubscribed " << subscriberId << " from " << topic << "\n";
    }

    int publish(const std::string &topic, const std::string &payload, DeliveryMode mode = DeliveryMode::SYNC) {
        const int count = publisher_.publish(topic, payload, mode);
        std::cout << "Published to '" << topic << "' | delivered=" << count << " subscribers\n";
        return count;
    }

    void printTopicInfo(const std::string &topic) const {
        if (!subscriptionService_.hasTopic(topic)) {
            std::cout << "Topic not found: " << topic << "\n";
            return;
        }
        const auto &subs = subscriptionService_.getSubscribers(topic);
        std::cout << "Topic '" << topic << "' has " << subs.size() << " subscriber(s):\n";
        for (const auto &s : subs) {
            std::cout << "  - " << s->getId() << "\n";
        }
    }

private:
    SubscriptionService subscriptionService_;
    DispatchService dispatchService_;
    Publisher publisher_;
    int messageCounter_ = 0;
};

} // namespace pub_sub_lld

#endif // PUB_SUB_SYSTEM_LLD_CORE_PUBSUBSYSTEM_H
