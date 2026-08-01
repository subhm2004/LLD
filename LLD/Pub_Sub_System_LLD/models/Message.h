#ifndef PUB_SUB_SYSTEM_LLD_MODELS_MESSAGE_H
#define PUB_SUB_SYSTEM_LLD_MODELS_MESSAGE_H

#include <chrono>
#include <string>

namespace pub_sub_lld {

struct Message {
    std::string messageId;
    std::string topic;
    std::string payload;
    long long publishedAtEpoch;

    static long long nowEpoch() {
        return std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }
};

} // namespace pub_sub_lld

#endif // PUB_SUB_SYSTEM_LLD_MODELS_MESSAGE_H
