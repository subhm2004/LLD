// ============================================================================
//  models/Location.h — lat/long coordinates + HAVERSINE distance 🌍
// ----------------------------------------------------------------------------
//  Do GPS coordinates ke beech ka REAL distance (km me) nikaalta hai.
//  "Nearby profiles within X km" discovery isi pe based hai.
//
//  ⭐ HAVERSINE FORMULA — ye interview me impress karta hai:
//  Do lat/long points ke beech distance nikaalne ke liye simple Pythagoras
//  (sqrt(dx²+dy²)) GALAT hoga — kyunki Earth GOL hai, flat nahi! Haversine
//  formula Earth ki curvature ko account karta hai (great-circle distance).
//    - lat/long ko radians me convert karo
//    - Earth radius (6371 km) use karo
//    - trigonometry (sin/cos/atan2) se curved-surface distance
//  Real Tinder/Uber/food-apps sab yahi (ya isse better) use karte hain
//  location-based features ke liye. Flat distance sirf chhote areas me chalti.
// ============================================================================
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
