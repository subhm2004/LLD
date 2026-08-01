#ifndef PUB_SUB_SYSTEM_LLD_SERVICES_SUBSCRIPTIONSERVICE_H
#define PUB_SUB_SYSTEM_LLD_SERVICES_SUBSCRIPTIONSERVICE_H

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../core/ISubscriber.h"

namespace pub_sub_lld {

class SubscriptionService {
public:
    void registerSubscriber(std::shared_ptr<ISubscriber> subscriber) {
        if (subscriber == nullptr) {
            throw std::invalid_argument("subscriber cannot be null");
        }
        const std::string &id = subscriber->getId();
        if (subscriberRegistry_.count(id)) {
            throw std::runtime_error("Subscriber already registered: " + id);
        }
        subscriberRegistry_[id] = subscriber;
    }

    void createTopic(const std::string &topic) {
        if (topic.empty()) {
            throw std::invalid_argument("topic cannot be empty");
        }
        topicSubscribers_[topic];
    }

    void subscribe(const std::string &topic, const std::string &subscriberId) {
        ensureTopic(topic);
        std::shared_ptr<ISubscriber> subscriber = getSubscriberOrThrow(subscriberId);

        std::vector<std::shared_ptr<ISubscriber>> &list = topicSubscribers_[topic];
        for (const auto &existing : list) {
            if (existing->getId() == subscriberId) {
                throw std::runtime_error("Already subscribed: " + subscriberId + " on " + topic);
            }
        }
        list.push_back(subscriber);
    }

    void unsubscribe(const std::string &topic, const std::string &subscriberId) {
        ensureTopic(topic);
        std::vector<std::shared_ptr<ISubscriber>> &list = topicSubscribers_[topic];
        for (auto it = list.begin(); it != list.end(); ++it) {
            if ((*it)->getId() == subscriberId) {
                list.erase(it);
                return;
            }
        }
        throw std::runtime_error("Subscriber not found on topic: " + subscriberId);
    }

    const std::vector<std::shared_ptr<ISubscriber>> &getSubscribers(const std::string &topic) const {
        static const std::vector<std::shared_ptr<ISubscriber>> empty;
        auto it = topicSubscribers_.find(topic);
        if (it == topicSubscribers_.end()) {
            return empty;
        }
        return it->second;
    }

    bool hasTopic(const std::string &topic) const { return topicSubscribers_.count(topic) > 0; }

private:
    std::unordered_map<std::string, std::shared_ptr<ISubscriber>> subscriberRegistry_;
    std::unordered_map<std::string, std::vector<std::shared_ptr<ISubscriber>>> topicSubscribers_;

    void ensureTopic(const std::string &topic) const {
        if (!hasTopic(topic)) {
            throw std::runtime_error("Topic does not exist: " + topic);
        }
    }

    std::shared_ptr<ISubscriber> getSubscriberOrThrow(const std::string &subscriberId) const {
        auto it = subscriberRegistry_.find(subscriberId);
        if (it == subscriberRegistry_.end()) {
            throw std::runtime_error("Subscriber not registered: " + subscriberId);
        }
        return it->second;
    }
};

} // namespace pub_sub_lld

#endif // PUB_SUB_SYSTEM_LLD_SERVICES_SUBSCRIPTIONSERVICE_H
