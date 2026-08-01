#ifndef RIDE_SHARING_APP_LLD_SERVICES_PRICINGSERVICE_H
#define RIDE_SHARING_APP_LLD_SERVICES_PRICINGSERVICE_H

#include "../models/Location.h"
#include "../utils/GeoUtils.h"

using namespace std;

namespace ride_sharing {

class PricingService {
public:
    double calculateFare(const Location &pickup, const Location &drop) const {
        const double baseFare = 40.0;
        const double perKm = 12.0;
        const double distance = calculateDistance(pickup, drop);
        return baseFare + (distance * perKm);
    }
};

} // namespace ride_sharing

#endif // RIDE_SHARING_APP_LLD_SERVICES_PRICINGSERVICE_H
