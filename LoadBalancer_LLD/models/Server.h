#ifndef LOADBALANCER_LLD_MODELS_SERVER_H
#define LOADBALANCER_LLD_MODELS_SERVER_H

#include <bits/stdc++.h>

#include "../enums/ServerStatus.h"

using namespace std;

namespace load_balancer_lld {

class Server {
public:
    Server(string serverId, string host)
        : serverId_(std::move(serverId)),
          host_(std::move(host)),
          status_(ServerStatus::UP),
          activeConnections_(0) {}

    const string &getId() const { return serverId_; }
    const string &getHost() const { return host_; }
    ServerStatus getStatus() const { return status_; }
    int getActiveConnections() const { return activeConnections_; }

    bool isAvailable() const { return status_ == ServerStatus::UP; }
    void setStatus(ServerStatus status) { status_ = status; }
    void incrementConnections() { ++activeConnections_; }
    void decrementConnections() {
        if (activeConnections_ > 0) {
            --activeConnections_;
        }
    }

private:
    string serverId_;
    string host_;
    ServerStatus status_;
    int activeConnections_;
};

} // namespace load_balancer_lld

#endif // LOADBALANCER_LLD_MODELS_SERVER_H
