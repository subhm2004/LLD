#ifndef ELEVATOR_SYSTEM_LLD_MODELS_ELEVATORCAR_H
#define ELEVATOR_SYSTEM_LLD_MODELS_ELEVATORCAR_H

#include <bits/stdc++.h>

#include "../enums/Direction.h"

using namespace std;

namespace elevator_lld {

class ElevatorCar {
public:
    ElevatorCar(int id, int minFloor, int maxFloor)
        : id_(id), minFloor_(minFloor), maxFloor_(maxFloor), currentFloor_(minFloor), direction_(Direction::IDLE) {}

    int getId() const { return id_; }
    int getCurrentFloor() const { return currentFloor_; }
    Direction getDirection() const { return direction_; }
    bool isIdle() const {
        return direction_ == Direction::IDLE && upStops_.empty() && downStops_.empty();
    }

    void addStop(int floor) {
        validateFloor(floor);
        if (floor > currentFloor_) {
            upStops_.insert(floor);
        } else if (floor < currentFloor_) {
            downStops_.insert(floor);
        }
    }

    void step() {
        if (upStops_.empty() && downStops_.empty()) {
            direction_ = Direction::IDLE;
            return;
        }

        if (direction_ == Direction::IDLE) {
            direction_ = pickInitialDirection();
        }

        if (direction_ == Direction::UP) {
            processUpDirection();
        } else {
            processDownDirection();
        }
    }

    void printState() const {
        cout << "Elevator#" << id_ << " | Floor: " << currentFloor_ << " | Direction: " << directionToString(direction_)
             << "\n";
    }

private:
    int id_;
    int minFloor_;
    int maxFloor_;
    int currentFloor_;
    Direction direction_;
    set<int> upStops_;
    set<int, greater<int>> downStops_;

    void validateFloor(int floor) const {
        if (floor < minFloor_ || floor > maxFloor_) {
            throw runtime_error("Invalid floor request: " + to_string(floor));
        }
    }

    Direction pickInitialDirection() const {
        if (!upStops_.empty() && !downStops_.empty()) {
            const int nearestUp = *upStops_.begin();
            const int nearestDown = *downStops_.begin();
            const int upDistance = abs(nearestUp - currentFloor_);
            const int downDistance = abs(nearestDown - currentFloor_);
            return (upDistance <= downDistance) ? Direction::UP : Direction::DOWN;
        }
        if (!upStops_.empty()) {
            return Direction::UP;
        }
        return Direction::DOWN;
    }

    void processUpDirection() {
        if (upStops_.empty()) {
            direction_ = downStops_.empty() ? Direction::IDLE : Direction::DOWN;
            return;
        }

        ++currentFloor_;
        auto it = upStops_.find(currentFloor_);
        if (it != upStops_.end()) {
            upStops_.erase(it);
            cout << "Elevator#" << id_ << " stopped at floor " << currentFloor_ << "\n";
        }

        if (upStops_.empty() && !downStops_.empty()) {
            direction_ = Direction::DOWN;
        } else if (upStops_.empty()) {
            direction_ = Direction::IDLE;
        }
    }

    void processDownDirection() {
        if (downStops_.empty()) {
            direction_ = upStops_.empty() ? Direction::IDLE : Direction::UP;
            return;
        }

        --currentFloor_;
        auto it = downStops_.find(currentFloor_);
        if (it != downStops_.end()) {
            downStops_.erase(it);
            cout << "Elevator#" << id_ << " stopped at floor " << currentFloor_ << "\n";
        }

        if (downStops_.empty() && !upStops_.empty()) {
            direction_ = Direction::UP;
        } else if (downStops_.empty()) {
            direction_ = Direction::IDLE;
        }
    }
};

} // namespace elevator_lld

#endif // ELEVATOR_SYSTEM_LLD_MODELS_ELEVATORCAR_H
