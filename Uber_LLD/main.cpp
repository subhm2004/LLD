#include <bits/stdc++.h>

#include "core/UberSystem.h"

using namespace std;
using namespace uber_lld;

int main() {
    UberSystem *uber = new UberSystem();

    uber->registerRider("R1", "Shubham", "9999999999");
    uber->registerRider("R2", "Ananya", "8888888888");

    uber->registerDriver("D1", "Amit", "DL01AB1234", {0.0, 0.0});
    uber->registerDriver("D2", "Ravi", "DL01CD5678", {2.5, 1.0});
    uber->registerDriver("D3", "Kunal", "DL01EF9012", {6.0, 6.0});

    string rideId = uber->requestRide("R1", {1.0, 1.2}, {8.0, 7.5});
    uber->startRide(rideId, 2234);
    uber->completeRide(rideId, PaymentMethod::UPI);

    string rideId2 = uber->requestRide("R2", {2.0, 2.0}, {5.0, 5.0});
    uber->cancelRide(rideId2);

    delete uber;
    return 0;
}
