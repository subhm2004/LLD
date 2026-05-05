#ifndef ELEVATOR_SYSTEM_LLD_ENUMS_DIRECTION_H
#define ELEVATOR_SYSTEM_LLD_ENUMS_DIRECTION_H

#include <bits/stdc++.h>

using namespace std;

namespace elevator_lld {

enum class Direction { UP, DOWN, IDLE };

inline string directionToString(Direction direction) {
    if (direction == Direction::UP) {
        return "UP";
    }
    if (direction == Direction::DOWN) {
        return "DOWN";
    }
    return "IDLE";
}

} // namespace elevator_lld

#endif // ELEVATOR_SYSTEM_LLD_ENUMS_DIRECTION_H
