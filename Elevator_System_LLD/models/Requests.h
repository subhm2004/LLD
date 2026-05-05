#ifndef ELEVATOR_SYSTEM_LLD_MODELS_REQUESTS_H
#define ELEVATOR_SYSTEM_LLD_MODELS_REQUESTS_H

#include <bits/stdc++.h>

#include "../enums/Direction.h"

using namespace std;

namespace elevator_lld {

struct ExternalRequest {
    int floor;
    Direction direction;
};

struct InternalRequest {
    int elevatorId;
    int destinationFloor;
};

} // namespace elevator_lld

#endif // ELEVATOR_SYSTEM_LLD_MODELS_REQUESTS_H
