// ============================================================================
//  models/ConnectionRequest.h  —  Ek connect hone ki request (Requirement R3)
// ----------------------------------------------------------------------------
//  Requirement R3 (Connection Management): "Users can send, cancel, accept, or
//  ignore connection requests, enabling them to curate their professional
//  network."
//
//  Ye ek DIRECTED request hai: FROM ne TO ko bheji. Iske paas ek status hota
//  hai jo iski poori kahani batata hai (ConnectionStatus.h dekho):
//
//     PENDING -> (accept)   -> ACCEPTED
//             -> (reject)   -> REJECTED
//             -> (ignore)   -> IGNORED    (TO user karta hai)
//             -> (cancel)   -> CANCELLED  (FROM user karta hai)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ye ek chhota STATE MACHINE hai — transition SIRF PENDING se hota hai  │
//  │                                                                          │
//  │  Ek baar request terminal state (ACCEPTED/REJECTED/CANCELLED/IGNORED) me  │
//  │  chali gayi, wo phir kabhi nahi badalti. Isse "already processed" wale    │
//  │  duplicate/invalid actions apne aap block ho jaate hain.                  │
//  │                                                                          │
//  │  Ye "guard" LinkedinSystem me lagaya gaya hai (accept/reject/cancel/     │
//  │  ignore se pehle status == PENDING check hota hai). Model sirf data +     │
//  │  ek simple setStatus rakhta hai — orchestration system ka kaam hai.       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `createdAt_` (timestamp) bhi store hota hai — aage "pending requests
//     inbox" ko time se sort karne ya purani requests expire karne me kaam aata.
// ============================================================================
#ifndef LINKEDIN_LLD_MODELS_CONNECTIONREQUEST_H
#define LINKEDIN_LLD_MODELS_CONNECTIONREQUEST_H

#include <ctime>
#include <string>
#include <utility>

#include "../enums/ConnectionStatus.h"

namespace linkedin_lld {

class ConnectionRequest {
public:
    ConnectionRequest(std::string requestId, std::string fromUserId, std::string toUserId)
        : requestId_(std::move(requestId)),
          fromUserId_(std::move(fromUserId)),
          toUserId_(std::move(toUserId)),
          status_(ConnectionStatus::PENDING), // har nayi request PENDING se shuru
          createdAt_(std::time(nullptr)) {}

    const std::string &getRequestId() const { return requestId_; }
    const std::string &getFromUserId() const { return fromUserId_; } // bhejne wala
    const std::string &getToUserId() const { return toUserId_; }     // paane wala
    ConnectionStatus getStatus() const { return status_; }
    long long getCreatedAt() const { return createdAt_; }

    // Status badalna — guard (PENDING check) system layer me hai, yahan nahi.
    void setStatus(ConnectionStatus status) { status_ = status; }

private:
    std::string requestId_;
    std::string fromUserId_;
    std::string toUserId_;
    ConnectionStatus status_;
    long long createdAt_;
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_MODELS_CONNECTIONREQUEST_H
