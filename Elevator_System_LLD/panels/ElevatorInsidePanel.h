// ============================================================================
//  panels/ElevatorInsidePanel.h  —  Elevator ke ANDAR ka panel (Req #14, #12)
// ----------------------------------------------------------------------------
//  Jab tum elevator me ghuste ho, saamne jo buttons ka board hota hai — wahi.
//  Ismein do tarah ke buttons hote hain:
//
//     1. FLOOR BUTTONS  -> 0, 1, 2, ... 14  ("mujhe 7th floor jaana hai")
//     2. DOOR BUTTONS   -> ◀▶ (open) aur ▶◀ (close)   [Requirement #12]
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Panel ek "REMOTE CONTROL" hai — usme koi logic NAHI hai              │
//  │                                                                          │
//  │  Dhyaan do ki ye class khud kuch DECIDE nahi karti:                      │
//  │    - "darwaza khul sakta hai kya?" -> ye ElevatorCar/Door decide karta   │
//  │    - "ye floor valid hai kya?"     -> ye ElevatorCar decide karta hai    │
//  │                                                                          │
//  │  Panel bas button dabne pe elevator ko BATA deta hai. Bilkul TV ke       │
//  │  remote jaisa — remote me TV nahi hota, wo bas signal bhejta hai.        │
//  │                                                                          │
//  │  Isse fayda: agar kal button ka behavior badle (jaise "floor button      │
//  │  dabane pe confirmation bhi maango"), to sirf yahan badlega. Aur agar    │
//  │  elevator ka logic badle, to panel ko chhuna hi nahi padega.             │
//  │  📌 Ye SINGLE RESPONSIBILITY hai — panel ka kaam sirf "input lena" hai.  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ OWNERSHIP: panel ke paas `ElevatorCar*` ka RAW pointer hai, par wo uska
//     MAALIK nahi hai — bas use "dekh" raha hai (borrow). Elevator ka maalik
//     ElevatorSystemController hai, wahi use delete karega. Panel kabhi nahi.
//     📌 "Pointer hone ka matlab maalik hona nahi hota."
// ============================================================================
#ifndef ELEVATOR_SYSTEM_LLD_PANELS_ELEVATORINSIDEPANEL_H
#define ELEVATOR_SYSTEM_LLD_PANELS_ELEVATORINSIDEPANEL_H

#include <iostream>

#include "../models/ElevatorCar.h"

using namespace std;

namespace elevator_lld {

class ElevatorInsidePanel {
public:
    // `explicit` -> chupke se `ElevatorCar*` ko panel me convert hone se rokta
    // hai. Single-argument constructor pe ye lagana acchi aadat hai.
    explicit ElevatorInsidePanel(ElevatorCar *elevator) : elevator_(elevator) {}

    // ---- FLOOR BUTTON (Requirement #14) ------------------------------------
    //
    // "Mujhe 7th floor jaana hai" — button dabaya.
    // Panel bas elevator ko stop add karne ko bolta hai. Floor valid hai ya
    // nahi, ye ElevatorCar khud check karta hai (aur galat pe throw karta hai).
    void pressFloorButton(int floor) {
        cout << "  [INSIDE PANEL | Elevator#" << elevator_->getId() << "] Floor button dabaya: " << floor << "\n";
        elevator_->addStop(floor);
    }

    // ---- DOOR OPEN BUTTON (Requirement #12) --------------------------------
    //
    // ⭐ Ye button HAMESHA kaam nahi karta — aur yahi sahi hai!
    // Agar elevator chal raha hai to `openDoor()` mana kar dega (Requirement
    // #11 ka safety rule). Panel usko force nahi kar sakta.
    //
    // Asli elevator me bhi aisa hi hota hai: chalte waqt open button dabao,
    // kuch nahi hota. Ruk jaao, phir dabao — khul jaayega.
    void pressOpenDoorButton() {
        cout << "  [INSIDE PANEL | Elevator#" << elevator_->getId() << "] Open-door button dabaya\n";
        elevator_->openDoor(); // andar safety check hai — mana bhi kar sakta hai
    }

    // ---- DOOR CLOSE BUTTON (Requirement #12) -------------------------------
    //
    // Band karna hamesha safe hai, isi liye yahan koi shart nahi.
    // (Door.h me likha hai: guard sirf wahan lagao jahan KHATRA hai.)
    void pressCloseDoorButton() {
        cout << "  [INSIDE PANEL | Elevator#" << elevator_->getId() << "] Close-door button dabaya\n";
        elevator_->closeDoor();
    }

    // ---- Andar wala display dikhao (Requirement #17) -----------------------
    void showDisplay() const { elevator_->showDisplays(); }

private:
    ElevatorCar *elevator_; // BORROWED — panel iska maalik nahi (upar note padho)
};

} // namespace elevator_lld

#endif // ELEVATOR_SYSTEM_LLD_PANELS_ELEVATORINSIDEPANEL_H
