#include <bits/stdc++.h>
#include "core/RideSharingService.h"

using namespace std;
using namespace ride_sharing;

int main() {
    RideSharingService *app = new RideSharingService();

    app->registerRider("R1", "Shubham");
    app->registerRider("R2", "Ananya");

    app->registerDriver("D1", "Amit", {0.0, 0.0});
    app->registerDriver("D2", "Ravi", {4.0, 4.0});
    app->registerDriver("D3", "Kunal", {2.0, 1.0});

    string rideId = app->requestRide("R1", {1.0, 1.0}, {8.0, 7.0});
    app->startRide(rideId);
    app->completeRide(rideId);

    delete app;
    return 0;
}
