#ifndef ELEVATOR_SYSTEM_LLD_CORE_ELEVATORSYSTEMCONTROLLER_H
#define ELEVATOR_SYSTEM_LLD_CORE_ELEVATORSYSTEMCONTROLLER_H

#include <bits/stdc++.h>

#include "../models/ElevatorCar.h"
#include "../models/Requests.h"
#include "../scheduler/ElevatorScheduler.h"

using namespace std;

namespace elevator_lld {

class ElevatorSystemController {
public:
    ElevatorSystemController(int totalElevators, int minFloor, int maxFloor) : minFloor_(minFloor), maxFloor_(maxFloor) {
        if (totalElevators <= 0 || minFloor >= maxFloor) {
            throw runtime_error("Invalid system configuration");
        }
        for (int id = 1; id <= totalElevators; ++id) {
            elevators_.push_back(new ElevatorCar(id, minFloor, maxFloor));
        }
    }

    ~ElevatorSystemController() {
        for (ElevatorCar *elevator : elevators_) {
            delete elevator;
        }
    }

    void requestElevator(const ExternalRequest &request) {
        validateFloor(request.floor);
        ElevatorCar *elevator = scheduler_.selectBestElevator(elevators_, request.floor);
        cout << "Assigning Elevator#" << elevator->getId() << " for external request at floor " << request.floor << " ("
             << directionToString(request.direction) << ")\n";
        elevator->addStop(request.floor);
    }

    void selectFloorInsideElevator(const InternalRequest &request) {
        validateFloor(request.destinationFloor);
        ElevatorCar *elevator = getElevatorById(request.elevatorId);
        cout << "Internal request in Elevator#" << request.elevatorId << " for floor " << request.destinationFloor
             << "\n";
        elevator->addStop(request.destinationFloor);
    }

    void runOneTick() {
        cout << "\n=== System Tick ===\n";
        for (const auto &elevator : elevators_) {
            elevator->step();
            elevator->printState();
        }
    }

private:
    int minFloor_;
    int maxFloor_;
    vector<ElevatorCar *> elevators_;
    ElevatorScheduler scheduler_;

    void validateFloor(int floor) const {
        if (floor < minFloor_ || floor > maxFloor_) {
            throw runtime_error("Floor out of range: " + to_string(floor));
        }
    }

    ElevatorCar *getElevatorById(int elevatorId) {
        if (elevatorId <= 0 || elevatorId > static_cast<int>(elevators_.size())) {
            throw runtime_error("Invalid elevator id: " + to_string(elevatorId));
        }
        return elevators_[elevatorId - 1];
    }
};

} // namespace elevator_lld

#endif // ELEVATOR_SYSTEM_LLD_CORE_ELEVATORSYSTEMCONTROLLER_H
