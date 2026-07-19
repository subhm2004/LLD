// ============================================================================
//  core/ElevatorSystemController.h  —  Poore system ka CONDUCTOR (Req #21)
// ----------------------------------------------------------------------------
//  Ye "elevator control system" hai — Requirement #21: system elevators ki
//  movement, darwazon ka operation, aur unke status ka dhyaan rakhta hai.
//
//  ⭐ Ye FACADE hai — client (main.cpp) ke liye ek hi simple darwaza:
//     Client ko ElevatorCar, Door, Display, Scheduler, Panel — kisi ka naam
//     tak nahi pata. Wo bas `controller->pressFloorButton(3, UP)` bolta hai,
//     aur andar poora tamasha ho jaata hai:
//        panel -> handler -> scheduler -> best elevator -> addStop
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  Controller ke paas kya-kya hai:                                         │
//  │     elevators_    -> saari elevator cars     (max 3, Req #8)             │
//  │     floorPanels_  -> har floor ka panel      (Req #15)                   │
//  │     insidePanels_ -> har elevator ka panel   (Req #14)                   │
//  │     scheduler_    -> best elevator chunta    (Req #22)                   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 OWNERSHIP saaf rakho — controller in SAB ka maalik hai:
//     - `elevators_` -> `new` kiye, destructor me `delete` karta hai
//     - `floorPanels_` / `insidePanels_` -> value me rakhe (vector<T>, T* nahi)
//       -> unka memory apne aap saaf ho jaata hai, koi delete likhna hi nahi
//       padta. Jahan raw pointer ki zaroorat na ho, mat use karo!
// ============================================================================
#ifndef ELEVATOR_SYSTEM_LLD_CORE_ELEVATORSYSTEMCONTROLLER_H
#define ELEVATOR_SYSTEM_LLD_CORE_ELEVATORSYSTEMCONTROLLER_H

#include <bits/stdc++.h>

#include "../config/SystemLimits.h"
#include "../models/ElevatorCar.h"
#include "../models/Requests.h"
#include "../panels/ElevatorInsidePanel.h"
#include "../panels/FloorPanel.h"
#include "../scheduler/ElevatorScheduler.h"

using namespace std;

namespace elevator_lld {

class ElevatorSystemController {
public:
    // ---- Building banao (Requirement #8 ki limits yahan lagti hain) --------
    //
    // `totalFloors` -> kitni floors (0 se totalFloors-1 tak). Max 15.
    // `totalElevators` -> kitne elevators. Max 3.
    ElevatorSystemController(int totalElevators, int totalFloors)
        : minFloor_(0), maxFloor_(totalFloors - 1) {

        // ---- ✅ Requirement #8 — BUILDING LIMITS ----
        //
        // "There's a cap of 15 floors in the building, with up to three
        //  elevators to service these floors."
        //
        // Pehle ye check tha hi nahi — koi bhi 100 floors ya 50 elevators bana
        // sakta tha. Ab limits enforce hoti hain, aur error message me saaf
        // likha hai ki kya galat hai aur limit kya hai.
        //
        // ⭐ Galti ko yahin rok do (constructor me) — "fail fast". Agar galat
        // config andar ghus gayi, to baad me kahin ajeeb jagah crash hoga aur
        // dhoondhne me ghante lagenge.
        if (totalElevators <= 0 || totalElevators > MAX_ELEVATORS) {
            throw runtime_error("Elevators 1 se " + to_string(MAX_ELEVATORS) + " ke beech hone chahiye (mile: " +
                                to_string(totalElevators) + ")");
        }
        if (totalFloors <= 1 || totalFloors > MAX_FLOORS) {
            throw runtime_error("Floors 2 se " + to_string(MAX_FLOORS) + " ke beech honi chahiye (mili: " +
                                to_string(totalFloors) + ")");
        }

        // ---- Elevator cars banao ----
        for (int id = 1; id <= totalElevators; ++id) {
            elevators_.push_back(new ElevatorCar(id, minFloor_, maxFloor_));
        }

        // ---- Har elevator ka INSIDE panel (Requirement #14) ----
        for (ElevatorCar *elevator : elevators_) {
            insidePanels_.emplace_back(elevator);
        }

        // ---- Har floor ka panel (Requirement #15) ----
        //
        // ⭐ Yahan LAMBDA se panel ko system se jodte hain. Panel ko controller
        // ka pointer nahi diya — bas ek function diya. (FloorPanel.h me poora
        // explanation hai ki isse circular dependency kaise tut-ti hai.)
        //
        // `[this]` -> lambda ke andar `this` (controller) use kar sakte hain.
        // ⚠ Iska matlab: lambda controller se zyada jeena nahi chahiye. Yahan
        // safe hai kyunki panels controller ke apne member hain — dono saath
        // hi khatam honge.
        for (int floor = minFloor_; floor <= maxFloor_; ++floor) {
            floorPanels_.emplace_back(floor, [this](int f, Direction d) {
                this->requestElevator({f, d});
            });
        }
    }

    ~ElevatorSystemController() {
        // Sirf `elevators_` delete karna hai — wahi `new` se bane the.
        // Panels value-type hain (vector<FloorPanel>, vector<...Panel>), unka
        // memory apne aap saaf hota hai.
        for (ElevatorCar *elevator : elevators_) {
            delete elevator;
        }
    }

    // ========================================================================
    //  EXTERNAL REQUEST — bahar se elevator bulana (Req #13, #22)
    // ========================================================================
    void requestElevator(const ExternalRequest &request) {
        validateFloor(request.floor);

        // ⭐ SMART DISPATCH (Requirement #22) — ab `direction` bhi bhejte hain.
        // Pehle sirf floor jaata tha, isi liye scheduler direction ignore karta
        // tha aur galat elevator chun sakta tha. (ElevatorScheduler.h padho.)
        ElevatorCar *elevator = scheduler_.selectBestElevator(elevators_, request.floor, request.direction);

        // Saare elevators full ho sakte hain (Requirement #20) -> nullptr.
        // Pehle ye case handle hi nahi tha -> seedha null-pointer crash.
        if (elevator == nullptr) {
            cout << "  ⚠ Koi elevator available nahi (sab full hain) — floor " << request.floor << " ka request wait me\n";
            return;
        }

        cout << "  → Elevator#" << elevator->getId() << " assign hua (floor " << request.floor << ", "
             << directionToString(request.direction) << ")\n";
        elevator->addStop(request.floor);
    }

    // ========================================================================
    //  INTERNAL REQUEST — andar se floor button (Req #14)
    // ========================================================================
    void selectFloorInsideElevator(const InternalRequest &request) {
        validateFloor(request.destinationFloor);
        ElevatorInsidePanel &panel = getInsidePanelById(request.elevatorId);
        panel.pressFloorButton(request.destinationFloor);
    }

    // ========================================================================
    //  DOOR CONTROL (Req #12) — inside panel ke through
    // ========================================================================
    void pressOpenDoor(int elevatorId) { getInsidePanelById(elevatorId).pressOpenDoorButton(); }
    void pressCloseDoor(int elevatorId) { getInsidePanelById(elevatorId).pressCloseDoorButton(); }

    // ========================================================================
    //  FLOOR PANEL buttons (Req #13, #15)
    // ========================================================================
    void pressFloorButton(int floor, Direction direction) {
        validateFloor(floor);
        // Panels index 0 se hain, aur floors bhi minFloor_ se — to `floor -
        // minFloor_` seedha index de deta hai.
        FloorPanel &panel = floorPanels_[floor - minFloor_];

        // Panel pe do hi buttons hain — UP aur DOWN (Requirement #13).
        // IDLE ka koi button nahi hota (koi "mujhe kahin nahi jaana" button
        // nahi dabata 😄), isi liye use reject kar dete hain.
        if (direction == Direction::UP) {
            panel.pressUp();
        } else if (direction == Direction::DOWN) {
            panel.pressDown();
        } else {
            throw runtime_error("Floor panel pe sirf UP/DOWN button hote hain, IDLE nahi");
        }
    }

    // ========================================================================
    //  CAPACITY (Req #19, #20)
    // ========================================================================
    bool boardPassenger(int elevatorId, int weightKg = DEFAULT_PASSENGER_WEIGHT_KG) {
        return getElevatorById(elevatorId)->boardPassenger(weightKg);
    }

    bool exitPassenger(int elevatorId, int weightKg = DEFAULT_PASSENGER_WEIGHT_KG) {
        return getElevatorById(elevatorId)->exitPassenger(weightKg);
    }

    // ========================================================================
    //  SIMULATION — ek tick aage
    // ========================================================================
    void runOneTick() {
        cout << "\n=== System Tick ===\n";
        for (ElevatorCar *elevator : elevators_) {
            elevator->step();
            elevator->printState();
        }
        // Har tick ke baad floor displays ko naya data bhejo (Requirement #18).
        refreshFloorDisplays();
    }

    // ---- Saare displays dikhao (Req #16, #17, #18) --------------------------
    void showAllDisplays() {
        cout << "\n--- Elevator Displays (andar + bahar) ---\n";
        for (const ElevatorCar *elevator : elevators_) {
            elevator->showDisplays();
        }
    }

    void showFloorDisplay(int floor) {
        validateFloor(floor);
        floorPanels_[floor - minFloor_].showDisplay();
    }

    // ---- Getters (testing/demo ke liye) ------------------------------------
    ElevatorCar *getElevator(int elevatorId) { return getElevatorById(elevatorId); }
    int getTotalElevators() const { return static_cast<int>(elevators_.size()); }
    int getTotalFloors() const { return maxFloor_ - minFloor_ + 1; }

private:
    int minFloor_;
    int maxFloor_;
    vector<ElevatorCar *> elevators_;         // controller MAALIK hai -> delete karta hai
    vector<ElevatorInsidePanel> insidePanels_; // value type -> apne aap saaf
    vector<FloorPanel> floorPanels_;           // value type -> apne aap saaf
    ElevatorScheduler scheduler_;

    void validateFloor(int floor) const {
        if (floor < minFloor_ || floor > maxFloor_) {
            throw runtime_error("Floor out of range: " + to_string(floor) + " (valid: " + to_string(minFloor_) + "-" +
                                to_string(maxFloor_) + ")");
        }
    }

    ElevatorCar *getElevatorById(int elevatorId) {
        if (elevatorId <= 0 || elevatorId > static_cast<int>(elevators_.size())) {
            throw runtime_error("Invalid elevator id: " + to_string(elevatorId));
        }
        return elevators_[elevatorId - 1];
    }

    ElevatorInsidePanel &getInsidePanelById(int elevatorId) {
        if (elevatorId <= 0 || elevatorId > static_cast<int>(insidePanels_.size())) {
            throw runtime_error("Invalid elevator id: " + to_string(elevatorId));
        }
        return insidePanels_[elevatorId - 1];
    }

    // ---- Floor displays ko update karo (Requirement #18) -------------------
    //
    // Har floor ka display "sabse nazdeek" elevator dikhata hai — kyunki floor
    // pe khade bande ko wahi elevator matter karta hai jo pehle aayega.
    void refreshFloorDisplays() {
        for (FloorPanel &panel : floorPanels_) {
            const ElevatorCar *nearest = nullptr;
            int bestDistance = numeric_limits<int>::max();
            for (const ElevatorCar *elevator : elevators_) {
                int distance = abs(elevator->getCurrentFloor() - panel.getFloorNumber());
                if (distance < bestDistance) {
                    bestDistance = distance;
                    nearest = elevator;
                }
            }
            if (nearest != nullptr) {
                panel.updateDisplay(nearest->getCurrentFloor(), nearest->getDirection());
            }
        }
    }
};

} // namespace elevator_lld

#endif // ELEVATOR_SYSTEM_LLD_CORE_ELEVATORSYSTEMCONTROLLER_H
