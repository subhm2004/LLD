// ============================================================================
//  panels/FloorPanel.h  —  Floor pe laga panel + display (Req #13, #15, #18)
// ----------------------------------------------------------------------------
//  Building ki har floor pe jo UP/DOWN ke do buttons hote hain — wahi. Saath
//  me ek display bhi hota hai jo batata hai elevator kahan hai.
//
//     Requirement #13 -> outside panel: elevator bulane ke buttons (UP/DOWN)
//     Requirement #15 -> har floor ka apna panel
//     Requirement #18 -> har floor ka apna display
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ye panel kisi EK elevator se juda NAHI hai — aur yahi asli baat hai  │
//  │                                                                          │
//  │  InsidePanel ek KHAAS elevator ka hota hai (jisme tum khade ho).         │
//  │  FloorPanel kisi bhi ek elevator ka nahi — wo poore SYSTEM se baat karta │
//  │  hai. Kyun?                                                              │
//  │                                                                          │
//  │  Kyunki jab tum 3rd floor pe UP button dabate ho, tum ye nahi kehte ki   │
//  │  "Elevator#2 bhejo" — tum bas kehte ho "koi bhi elevator bhejo!" Ab      │
//  │  kaunsa aaye, ye SYSTEM decide karta hai (smart dispatch, Req #22).      │
//  │                                                                          │
//  │  Isi liye is panel ke paas ek callback (`RequestHandler`) hai, kisi      │
//  │  elevator ka pointer nahi.                                               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 CALLBACK kyun, seedha `ElevatorSystemController*` kyun nahi?
//     Kyunki tab CIRCULAR DEPENDENCY ban jaati: Controller ko FloorPanel
//     chahiye (wo unhe banata hai), aur FloorPanel ko Controller chahiye. Dono
//     ek doosre ko include karte -> murgi-anda problem.
//
//     `std::function` se ye poori tarah tut jaati hai: panel ko sirf itna pata
//     hai ki "ek function hai jise call karna hai" — kiska function hai, kahan
//     se aaya, kuch nahi pata. Controller usko wire kar deta hai.
//
//     Bonus: test me asli controller ki zaroorat hi nahi — ek fake lambda
//     bhej do aur panel ko akele test kar lo.
// ============================================================================
#ifndef ELEVATOR_SYSTEM_LLD_PANELS_FLOORPANEL_H
#define ELEVATOR_SYSTEM_LLD_PANELS_FLOORPANEL_H

#include <functional>
#include <iostream>
#include <string>

#include "../enums/Direction.h"
#include "../models/Display.h"

using namespace std;

namespace elevator_lld {

class FloorPanel {
public:
    // Jab button dabe to kya karna hai — ye "callback" batata hai.
    // (floor, direction) -> void
    using RequestHandler = function<void(int floor, Direction direction)>;

    FloorPanel(int floorNumber, RequestHandler handler)
        : floorNumber_(floorNumber), handler_(std::move(handler)), display_(DisplayType::FLOOR) {}

    // ---- UP BUTTON (Requirement #13) ---------------------------------------
    // "Mujhe UPAR jaana hai" — system ko bata do.
    void pressUp() {
        cout << "  [FLOOR PANEL | Floor " << floorNumber_ << "] ▲ UP button dabaya\n";
        handler_(floorNumber_, Direction::UP);
    }

    // ---- DOWN BUTTON (Requirement #13) -------------------------------------
    void pressDown() {
        cout << "  [FLOOR PANEL | Floor " << floorNumber_ << "] ▼ DOWN button dabaya\n";
        handler_(floorNumber_, Direction::DOWN);
    }

    // ---- FLOOR DISPLAY (Requirement #18) -----------------------------------
    //
    // Floor pe khade bande ko dikhana ki elevator abhi kahan hai aur kis taraf
    // ja raha hai — taaki wo decide kar sake ki intezaar kare ya seedhi le. 😄
    void updateDisplay(int elevatorFloor, Direction elevatorDirection) {
        // people/weight 0 bhej rahe hain — FLOOR display unhe dikhata hi nahi
        // (sirf INSIDE display capacity dikhata hai, Requirement #17).
        display_.update(elevatorFloor, elevatorDirection, 0, 0);
    }

    void showDisplay() const { display_.show("Floor " + to_string(floorNumber_)); }

    int getFloorNumber() const { return floorNumber_; }

private:
    int floorNumber_;
    RequestHandler handler_; // system ko request bhejne ka raasta (upar note padho)
    Display display_;        // #18
};

} // namespace elevator_lld

#endif // ELEVATOR_SYSTEM_LLD_PANELS_FLOORPANEL_H
