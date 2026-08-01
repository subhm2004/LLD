#ifndef UBER_LLD_SERVICES_DRIVERMATCHINGSERVICE_H
#define UBER_LLD_SERVICES_DRIVERMATCHINGSERVICE_H

#include <bits/stdc++.h>

#include "../enums/DriverStatus.h"
#include "../models/Driver.h"
#include "../models/Location.h"

using namespace std;

namespace uber_lld {

class DriverMatchingService {
public:
    Driver *findNearestAvailableDriver(const unordered_map<string, Driver *> &drivers, const Location &pickup) const {
        Driver *bestDriver = nullptr;
        double bestDistance = numeric_limits<double>::max();

        for (const auto &entry : drivers) {
            Driver *driver = entry.second;
            if (driver->getStatus() != DriverStatus::AVAILABLE) {
                continue;
            }

            const double distance = distanceBetween(driver->getLocation(), pickup);
            if (distance < bestDistance) {
                bestDistance = distance;
                bestDriver = driver;
            }
        }
        return bestDriver;
    }

private:
    static double distanceBetween(const Location &a, const Location &b) {
        const double dx = a.x - b.x;
        const double dy = a.y - b.y;
        return sqrt(dx * dx + dy * dy);
    }
};

} // namespace uber_lld

#endif // UBER_LLD_SERVICES_DRIVERMATCHINGSERVICE_H
