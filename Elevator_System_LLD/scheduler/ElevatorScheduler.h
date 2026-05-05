#ifndef ELEVATOR_SYSTEM_LLD_SCHEDULER_ELEVATORSCHEDULER_H
#define ELEVATOR_SYSTEM_LLD_SCHEDULER_ELEVATORSCHEDULER_H

#include <bits/stdc++.h>

#include "../models/ElevatorCar.h"

using namespace std;

namespace elevator_lld {

class ElevatorScheduler {
public:
    ElevatorCar *selectBestElevator(const vector<ElevatorCar *> &elevators, int requestFloor) {
        ElevatorCar *best = nullptr;
        int bestScore = numeric_limits<int>::max();

        for (const auto &elevator : elevators) {
            int score = abs(elevator->getCurrentFloor() - requestFloor);
            if (!elevator->isIdle()) {
                score += 2;
            }
            if (score < bestScore) {
                bestScore = score;
                best = elevator;
            }
        }
        return best;
    }
};

} // namespace elevator_lld

#endif // ELEVATOR_SYSTEM_LLD_SCHEDULER_ELEVATORSCHEDULER_H
