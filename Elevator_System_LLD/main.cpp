#include <bits/stdc++.h>

#include "core/ElevatorSystemController.h"

using namespace std;
using namespace elevator_lld;

int main() {
    ElevatorSystemController *controller = new ElevatorSystemController(3, 0, 10);

    controller->requestElevator({3, Direction::UP});
    controller->requestElevator({7, Direction::DOWN});
    controller->requestElevator({1, Direction::UP});

    controller->runOneTick();
    controller->runOneTick();

    controller->selectFloorInsideElevator({1, 9});
    controller->selectFloorInsideElevator({2, 0});

    for (int i = 0; i < 8; i++) {
        controller->runOneTick();
    }

    delete controller;
    return 0;
}
