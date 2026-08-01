#ifndef PUB_SUB_SYSTEM_LLD_SUBSCRIBERS_ANALYTICSSUBSCRIBER_H
#define PUB_SUB_SYSTEM_LLD_SUBSCRIBERS_ANALYTICSSUBSCRIBER_H

#include <iostream>
#include <string>
#include <unordered_map>

#include "../core/ISubscriber.h"

namespace pub_sub_lld {

class AnalyticsSubscriber : public ISubscriber {
public:
    explicit AnalyticsSubscriber(std::string id) : id_(std::move(id)) {}

    std::string getId() const override { return id_; }

    void onMessage(const Message &message) override {
        eventCountByTopic_[message.topic]++;
        std::cout << "[ANALYTICS][" << id_ << "] recorded event on '" << message.topic
                  << "' (total=" << eventCountByTopic_[message.topic] << ")\n";
    }

    int getCount(const std::string &topic) const {
        auto it = eventCountByTopic_.find(topic);
        return it == eventCountByTopic_.end() ? 0 : it->second;
    }

private:
    std::string id_;
    std::unordered_map<std::string, int> eventCountByTopic_;
};

} // namespace pub_sub_lld

#endif // PUB_SUB_SYSTEM_LLD_SUBSCRIBERS_ANALYTICSSUBSCRIBER_H
