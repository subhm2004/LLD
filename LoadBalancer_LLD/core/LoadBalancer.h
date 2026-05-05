#ifndef LOADBALANCER_LLD_CORE_LOADBALANCER_H
#define LOADBALANCER_LLD_CORE_LOADBALANCER_H

#include <bits/stdc++.h>

#include "../enums/ServerStatus.h"
#include "../models/Server.h"
#include "../strategies/LoadBalancingStrategy.h"

using namespace std;

namespace load_balancer_lld {

class LoadBalancer {
public:
    explicit LoadBalancer(unique_ptr<LoadBalancingStrategy> strategy) : strategy_(std::move(strategy)) {}

    ~LoadBalancer() {
        for (Server *server : servers_) {
            delete server;
        }
    }

    void addServer(const string &serverId, const string &host) {
        if (serverById_.count(serverId) != 0) {
            throw runtime_error("Server already exists");
        }
        Server *server = new Server(serverId, host);
        servers_.push_back(server);
        serverById_[serverId] = server;
    }

    void markServerStatus(const string &serverId, ServerStatus status) {
        Server *server = getServerOrThrow(serverId);
        server->setStatus(status);
    }

    Server *routeRequest() {
        Server *selected = strategy_->selectServer(servers_);
        if (selected == nullptr) {
            throw runtime_error("No available server to route request");
        }
        selected->incrementConnections();
        return selected;
    }

    void completeRequest(const string &serverId) {
        Server *server = getServerOrThrow(serverId);
        server->decrementConnections();
    }

    void setStrategy(unique_ptr<LoadBalancingStrategy> strategy) { strategy_ = std::move(strategy); }

    void printServerStats() const {
        cout << "\n=== Server Stats ===\n";
        for (const Server *server : servers_) {
            cout << server->getId() << " | " << server->getHost()
                 << " | Status: " << (server->getStatus() == ServerStatus::UP ? "UP" : "DOWN")
                 << " | ActiveConnections: " << server->getActiveConnections() << "\n";
        }
    }

private:
    vector<Server *> servers_;
    unordered_map<string, Server *> serverById_;
    unique_ptr<LoadBalancingStrategy> strategy_;

    Server *getServerOrThrow(const string &serverId) const {
        auto it = serverById_.find(serverId);
        if (it == serverById_.end()) {
            throw runtime_error("Server not found: " + serverId);
        }
        return it->second;
    }
};

} // namespace load_balancer_lld

#endif // LOADBALANCER_LLD_CORE_LOADBALANCER_H
