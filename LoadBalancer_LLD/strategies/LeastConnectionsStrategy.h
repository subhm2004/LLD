#ifndef LOADBALANCER_LLD_STRATEGIES_LEASTCONNECTIONSSTRATEGY_H
#define LOADBALANCER_LLD_STRATEGIES_LEASTCONNECTIONSSTRATEGY_H

#include <bits/stdc++.h>

#include "LoadBalancingStrategy.h"

using namespace std;

namespace load_balancer_lld {

class LeastConnectionsStrategy : public LoadBalancingStrategy {
public:
    Server *selectServer(const vector<Server *> &servers) override {
        Server *best = nullptr;
        int minConnections = numeric_limits<int>::max();

        for (Server *server : servers) {
            if (!server->isAvailable()) {
                continue;
            }
            if (server->getActiveConnections() < minConnections) {
                minConnections = server->getActiveConnections();
                best = server;
            }
        }
        return best;
    }
};

} // namespace load_balancer_lld

#endif // LOADBALANCER_LLD_STRATEGIES_LEASTCONNECTIONSSTRATEGY_H
