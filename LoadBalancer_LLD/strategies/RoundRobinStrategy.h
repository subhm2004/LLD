#ifndef LOADBALANCER_LLD_STRATEGIES_ROUNDROBINSTRATEGY_H
#define LOADBALANCER_LLD_STRATEGIES_ROUNDROBINSTRATEGY_H

#include <bits/stdc++.h>

#include "LoadBalancingStrategy.h"

using namespace std;

namespace load_balancer_lld {

class RoundRobinStrategy : public LoadBalancingStrategy {
public:
    RoundRobinStrategy() : nextIndex_(0) {}

    Server *selectServer(const vector<Server *> &servers) override {
        if (servers.empty()) {
            return nullptr;
        }

        const size_t n = servers.size();
        for (size_t i = 0; i < n; ++i) {
            const size_t index = (nextIndex_ + i) % n;
            if (servers[index]->isAvailable()) {
                nextIndex_ = (index + 1) % n;
                return servers[index];
            }
        }
        return nullptr;
    }

private:
    size_t nextIndex_;
};

} // namespace load_balancer_lld

#endif // LOADBALANCER_LLD_STRATEGIES_ROUNDROBINSTRATEGY_H
