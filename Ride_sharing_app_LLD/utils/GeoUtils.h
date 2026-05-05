#ifndef RIDE_SHARING_APP_LLD_UTILS_GEOUTILS_H
#define RIDE_SHARING_APP_LLD_UTILS_GEOUTILS_H

#include <cmath>

#include "../models/Location.h"

using namespace std;

namespace ride_sharing {

inline double calculateDistance(const Location &a, const Location &b) {
    const double dx = a.x - b.x;
    const double dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

} // namespace ride_sharing

#endif // RIDE_SHARING_APP_LLD_UTILS_GEOUTILS_H
