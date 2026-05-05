#ifndef LOADBALANCER_LLD_STRATEGIES_LOADBALANCINGSTRATEGY_H
#define LOADBALANCER_LLD_STRATEGIES_LOADBALANCINGSTRATEGY_H

#include <bits/stdc++.h>

#include "../models/Server.h"

using namespace std;

namespace load_balancer_lld {

class LoadBalancingStrategy {
public:
    virtual ~LoadBalancingStrategy() = default;
    virtual Server *selectServer(const vector<Server *> &servers) = 0;
};

} // namespace load_balancer_lld

#endif // LOADBALANCER_LLD_STRATEGIES_LOADBALANCINGSTRATEGY_H
