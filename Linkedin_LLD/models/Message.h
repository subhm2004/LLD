// ============================================================================
//  models/Message.h  —  Ek direct message (Requirement R9)
// ----------------------------------------------------------------------------
//  Requirement R9 (Messaging): "Direct communication is facilitated through
//  messaging, allowing users to send and receive messages with other
//  professionals."
//
//  Ek message ek chhoti immutable cheez hai: kisne (from), kise (to), kya
//  (content), aur kab (createdAt). Bas.
//
//  📌 Messages ka STORAGE MessagingService karti hai (kaunsa message kiske
//     inbox me — dono taraf). Ye class sirf ek message ke data ko represent
//     karti hai (SRP: model = data, service = logic).
// ============================================================================
#ifndef LINKEDIN_LLD_MODELS_MESSAGE_H
#define LINKEDIN_LLD_MODELS_MESSAGE_H

#include <ctime>
#include <string>
#include <utility>

namespace linkedin_lld {

class Message {
public:
    Message(std::string messageId, std::string fromUserId, std::string toUserId, std::string content)
        : messageId_(std::move(messageId)),
          fromUserId_(std::move(fromUserId)),
          toUserId_(std::move(toUserId)),
          content_(std::move(content)),
          createdAt_(std::time(nullptr)) {}

    const std::string &getMessageId() const { return messageId_; }
    const std::string &getFromUserId() const { return fromUserId_; }
    const std::string &getToUserId() const { return toUserId_; }
    const std::string &getContent() const { return content_; }
    long long getCreatedAt() const { return createdAt_; }

private:
    std::string messageId_;
    std::string fromUserId_;
    std::string toUserId_;
    std::string content_;
    long long createdAt_;
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_MODELS_MESSAGE_H
