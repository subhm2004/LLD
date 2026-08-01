#ifndef PUB_SUB_SYSTEM_LLD_CORE_ISUBSCRIBER_H
#define PUB_SUB_SYSTEM_LLD_CORE_ISUBSCRIBER_H

#include <string>

#include "../models/Message.h"

namespace pub_sub_lld {

class ISubscriber {
public:
    virtual ~ISubscriber() = default;
    virtual std::string getId() const = 0;
    virtual void onMessage(const Message &message) = 0;
};

} // namespace pub_sub_lld

#endif // PUB_SUB_SYSTEM_LLD_CORE_ISUBSCRIBER_H
