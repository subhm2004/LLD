// models/Location.h — lat/long coordinates + do locations ke beech distance.
// "Nearby profiles within X km" discovery isi se calculate hoti hai.
#ifndef TINDER_LLD_MODELS_LOCATION_H
#define TINDER_LLD_MODELS_LOCATION_H

#include <bits/stdc++.h>

using namespace std;

namespace tinder_lld {

struct Location {
    double latitude;
    double longitude;

    double distanceInKm(const Location &other) const {
        const double earthRadiusKm = 6371.0;
        const double dLat = (other.latitude - latitude) * M_PI / 180.0;
        const double dLon = (other.longitude - longitude) * M_PI / 180.0;
        const double a = sin(dLat / 2) * sin(dLat / 2) +
                         cos(latitude * M_PI / 180.0) * cos(other.latitude * M_PI / 180.0) *
                             sin(dLon / 2) * sin(dLon / 2);
        const double c = 2 * atan2(sqrt(a), sqrt(1 - a));
        return earthRadiusKm * c;
    }
};

} // namespace tinder_lld

#endif // TINDER_LLD_MODELS_LOCATION_H
