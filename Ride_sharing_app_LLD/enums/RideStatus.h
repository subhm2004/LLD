#ifndef RIDE_SHARING_APP_LLD_ENUMS_RIDESTATUS_H
#define RIDE_SHARING_APP_LLD_ENUMS_RIDESTATUS_H

#include <string>

using namespace std;

namespace ride_sharing {

enum class RideStatus { REQUESTED, ACCEPTED, STARTED, COMPLETED, CANCELLED };

} // namespace ride_sharing

#endif // RIDE_SHARING_APP_LLD_ENUMS_RIDESTATUS_H
