// ============================================================================
//  main.cpp  —  Load Balancer ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Ye demo Strategy pattern ka poora maza dikhata hai:
//    1. Round Robin routing (baari-baari)
//    2. Ek server DOWN mark karke dikhana ki routing use skip kar deti hai
//    3. RUNTIME pe strategy switch (RoundRobin -> LeastConnections) — bina kuch
//       aur badle, routing ka behavior turant badal jaata hai
//
//  ⭐ Client ko sirf `LoadBalancer` dikhta hai (Facade). Strategy classes ka
//     naam bas construct karte waqt aata hai; baaki routing logic hidden hai.
// ============================================================================
#include <iostream>
#include <memory>

#include "core/LoadBalancer.h"
#include "strategies/LeastConnectionsStrategy.h"
#include "strategies/RoundRobinStrategy.h"

using namespace std;
using namespace load_balancer_lld;

int main() {
    // Round Robin strategy inject karke LoadBalancer banaya.
    LoadBalancer loadBalancer(make_unique<RoundRobinStrategy>());

    // 3 backend servers onboard (sab shuru me UP, 0 connections)
    loadBalancer.addServer("S1", "10.0.0.1:8080");
    loadBalancer.addServer("S2", "10.0.0.2:8080");
    loadBalancer.addServer("S3", "10.0.0.3:8080");

    // ---- Round Robin: 5 requests -> S1, S2, S3, S1, S2 (baari-baari) --------
    cout << "=== Round Robin Routing ===\n";
    for (int i = 1; i <= 5; ++i) {
        Server *server = loadBalancer.routeRequest();
        cout << "Request " << i << " -> " << server->getId() << "\n";
    }

    loadBalancer.printServerStats(); // S1=2, S2=2, S3=1 connections

    // ---- Health: S2 DOWN -> ab routing S2 ko SKIP karegi --------------------
    loadBalancer.markServerStatus("S2", ServerStatus::DOWN);
    cout << "\nMarked S2 DOWN\n";

    // Request 6: round robin ka agla turn S3 hoga (S2 down, skip nahi... dekho)
    Server *server6 = loadBalancer.routeRequest();
    cout << "Request 6 -> " << server6->getId() << "\n";

    // ---- ⭐ RUNTIME STRATEGY SWITCH -> Least Connections --------------------
    loadBalancer.setStrategy(make_unique<LeastConnectionsStrategy>());
    cout << "\nSwitched strategy to Least Connections\n";

    // Ab jo server sabse kam bhara hai (UP me) usko request milegi
    Server *server7 = loadBalancer.routeRequest();
    Server *server8 = loadBalancer.routeRequest();
    cout << "Request 7 -> " << server7->getId() << "\n";
    cout << "Request 8 -> " << server8->getId() << "\n";

    // ---- Kuch requests complete -> un servers ka load ghatega --------------
    loadBalancer.completeRequest("S1");
    loadBalancer.completeRequest("S3");

    loadBalancer.printServerStats();
    return 0;
    // ⭐ Koi `delete` nahi — loadBalancer stack pe hai, scope khatam hote hi
    //    uska destructor saare Server* aur strategy ko saaf kar dega.
}
