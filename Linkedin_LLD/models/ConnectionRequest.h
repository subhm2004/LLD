#ifndef LINKEDIN_LLD_MODELS_CONNECTIONREQUEST_H
#define LINKEDIN_LLD_MODELS_CONNECTIONREQUEST_H

#include <bits/stdc++.h>

#include "../enums/ConnectionStatus.h"

using namespace std;

namespace linkedin_lld {

class ConnectionRequest {
public:
    ConnectionRequest(string requestId, string fromUserId, string toUserId)
        : requestId_(std::move(requestId)),
          fromUserId_(std::move(fromUserId)),
          toUserId_(std::move(toUserId)),
          status_(ConnectionStatus::PENDING),
          createdAt_(time(nullptr)) {}

    const string &getRequestId() const { return requestId_; }
    const string &getFromUserId() const { return fromUserId_; }
    const string &getToUserId() const { return toUserId_; }
    ConnectionStatus getStatus() const { return status_; }
    long long getCreatedAt() const { return createdAt_; }

    void setStatus(ConnectionStatus status) { status_ = status; }

private:
    string requestId_;
    string fromUserId_;
    string toUserId_;
    ConnectionStatus status_;
    long long createdAt_;
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_MODELS_CONNECTIONREQUEST_H
