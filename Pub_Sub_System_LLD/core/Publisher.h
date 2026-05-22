#ifndef PUB_SUB_SYSTEM_LLD_CORE_PUBLISHER_H
#define PUB_SUB_SYSTEM_LLD_CORE_PUBLISHER_H

#include <string>

#include "../enums/DeliveryMode.h"
#include "../models/Message.h"
#include "../services/DispatchService.h"

namespace pub_sub_lld {

class Publisher {
public:
    explicit Publisher(DispatchService &dispatch, int &messageCounter)
        : dispatch_(dispatch), messageCounter_(messageCounter) {}

    int publish(const std::string &topic, const std::string &payload,
                DeliveryMode mode = DeliveryMode::SYNC) {
        Message message{"MSG_" + std::to_string(++messageCounter_), topic, payload, Message::nowEpoch()};
        return dispatch_.publish(message, mode);
    }

private:
    DispatchService &dispatch_;
    int &messageCounter_;
};

} // namespace pub_sub_lld

#endif // PUB_SUB_SYSTEM_LLD_CORE_PUBLISHER_H
