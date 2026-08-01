#ifndef UBER_LLD_SERVICES_FARESERVICE_H
#define UBER_LLD_SERVICES_FARESERVICE_H

#include <bits/stdc++.h>

#include "../models/Location.h"

using namespace std;

namespace uber_lld {

struct FareDetails {
    double baseFare;
    double distanceKm;
    double perKmRate;
    double totalFare;
};

class FareService {
public:
    FareDetails calculateFareDetails(const Location &pickup, const Location &drop) const {
        const double baseFare = 50.0;
        const double perKm = 14.0;
        const double distance = distanceBetween(pickup, drop);
        FareDetails details{baseFare, distance, perKm, baseFare + (distance * perKm)};
        return details;
    }

    double calculateFare(const Location &pickup, const Location &drop) const {
        return calculateFareDetails(pickup, drop).totalFare;
    }

private:
    static double distanceBetween(const Location &a, const Location &b) {
        const double dx = a.x - b.x;
        const double dy = a.y - b.y;
        return sqrt(dx * dx + dy * dy);
    }
};

} // namespace uber_lld

#endif // UBER_LLD_SERVICES_FARESERVICE_H
