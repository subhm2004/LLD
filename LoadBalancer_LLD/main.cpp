#include <bits/stdc++.h>

#include "core/LoadBalancer.h"
#include "strategies/LeastConnectionsStrategy.h"
#include "strategies/RoundRobinStrategy.h"

using namespace std;
using namespace load_balancer_lld;

int main() {
    LoadBalancer loadBalancer(make_unique<RoundRobinStrategy>());

    loadBalancer.addServer("S1", "10.0.0.1:8080");
    loadBalancer.addServer("S2", "10.0.0.2:8080");
    loadBalancer.addServer("S3", "10.0.0.3:8080");

    cout << "=== Round Robin Routing ===\n";
    for (int i = 1; i <= 5; ++i) {
        Server *server = loadBalancer.routeRequest();
        cout << "Request " << i << " -> " << server->getId() << "\n";
    }

    loadBalancer.printServerStats();

    loadBalancer.markServerStatus("S2", ServerStatus::DOWN);
    cout << "\nMarked S2 DOWN\n";

    Server *server6 = loadBalancer.routeRequest();
    cout << "Request 6 -> " << server6->getId() << "\n";

    loadBalancer.setStrategy(make_unique<LeastConnectionsStrategy>());
    cout << "\nSwitched strategy to Least Connections\n";

    Server *server7 = loadBalancer.routeRequest();
    Server *server8 = loadBalancer.routeRequest();
    cout << "Request 7 -> " << server7->getId() << "\n";
    cout << "Request 8 -> " << server8->getId() << "\n";

    loadBalancer.completeRequest("S1");
    loadBalancer.completeRequest("S3");

    loadBalancer.printServerStats();
    return 0;
}
