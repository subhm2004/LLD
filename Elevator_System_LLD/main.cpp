// ============================================================================
//  main.cpp  —  Elevator System ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Ye file poore system ki "kahani" hai. Har scene ek ya do requirements ko
//  chala ke DIKHATA hai — sirf likha nahi, asli output me dikhta hai.
//
//  ⭐ CLIENT KITNA KAM JAANTA HAI (Facade ka fayda):
//     System me 8+ classes hain — ElevatorCar, Door, Display, FloorPanel,
//     ElevatorInsidePanel, ElevatorScheduler, waghairah. Yahan kitni dikhti
//     hain? Sirf EK: `ElevatorSystemController`. Baaki sab uske peeche chhupi
//     hain. Client bas `controller->...` bolta hai.
//
//  ============================================================================
//   DEMO ka plan — kaunsa scene kaunsi requirement dikhata hai
//  ----------------------------------------------------------------------------
//     Scene 0  -> #8   Building limits (15 floors / 3 elevators ka cap)
//     Scene 1  -> #13, #15, #22  Floor panel se call + smart dispatch
//     Scene 2  -> #9, #10  Movement (UP/DOWN/IDLE), har floor pe stop
//     Scene 3  -> #11  Chalti elevator ka darwaza NAHI khulta (safety)
//     Scene 4  -> #11, #12  Ruki elevator ka darwaza khulta hai
//     Scene 5  -> #19, #20  Capacity — 8 log / 680 kg
//     Scene 6  -> #16, #17, #18  Saare displays
//     Scene 7  -> #14, #23  Inside panel + multiple passengers
//  ============================================================================
#include <bits/stdc++.h>

#include "core/ElevatorSystemController.h"

using namespace std;
using namespace elevator_lld;

// Chhota helper — har scene ka heading saaf dikhe.
static void scene(const string &title) {
    cout << "\n\n╔══════════════════════════════════════════════════════════════╗\n";
    cout << "  " << title << "\n";
    cout << "╚══════════════════════════════════════════════════════════════╝\n";
}

int main() {
    // ========================================================================
    //  SCENE 0 — BUILDING LIMITS (Requirement #8)
    //  "cap of 15 floors, up to three elevators"
    // ========================================================================
    scene("SCENE 0 — Building limits (Req #8): 15 floors / 3 elevators ka cap");

    // Galat config reject honi chahiye — try karke dikhate hain.
    try {
        ElevatorSystemController bad(5, 10); // 5 elevators? limit 3 hai!
        cout << "❌ Ye nahi hona chahiye tha\n";
    } catch (const exception &e) {
        cout << "✅ 5 elevators reject: " << e.what() << "\n";
    }

    try {
        ElevatorSystemController bad(3, 20); // 20 floors? limit 15 hai!
        cout << "❌ Ye nahi hona chahiye tha\n";
    } catch (const exception &e) {
        cout << "✅ 20 floors reject: " << e.what() << "\n";
    }

    // Sahi config — 3 elevators, 15 floors (0 se 14 tak)
    ElevatorSystemController *controller = new ElevatorSystemController(3, 15);
    cout << "✅ Building bani: " << controller->getTotalElevators() << " elevators, "
         << controller->getTotalFloors() << " floors (0-14)\n";

    // ========================================================================
    //  SCENE 1 — FLOOR PANEL + SMART DISPATCH (Requirement #13, #15, #22)
    // ========================================================================
    scene("SCENE 1 — Floor panel se call (Req #13, #15) + smart dispatch (Req #22)");

    // Har floor ka apna panel hai. Uske UP/DOWN button dabao, system khud
    // best elevator chunega — position AUR direction dekh ke.
    controller->pressFloorButton(3, Direction::UP);
    controller->pressFloorButton(7, Direction::DOWN);
    controller->pressFloorButton(1, Direction::UP);

    // ========================================================================
    //  SCENE 2 — MOVEMENT (Requirement #9, #10)
    // ========================================================================
    scene("SCENE 2 — Movement: UP / DOWN / IDLE (Req #9), har floor pe stop (Req #10)");

    controller->runOneTick();
    controller->runOneTick();

    // ========================================================================
    //  SCENE 3 — DOOR SAFETY (Requirement #11) ⭐ sabse zaroori
    //  "doors can only open when it's not moving, in its idle state"
    // ========================================================================
    scene("SCENE 3 — ⭐ Chalti elevator ka darwaza NAHI khulta (Req #11)");

    ElevatorCar *e1 = controller->getElevator(1);
    cout << "Elevator#1 ki abhi haalat: ";
    e1->printState();

    if (e1->getDirection() != Direction::IDLE) {
        cout << "\nElevator CHAL raha hai — ab open-door button dabate hain:\n";
        controller->pressOpenDoor(1);
        cout << (e1->isDoorOpen() ? "❌ BUG! Darwaza khul gaya!\n" : "✅ Darwaza band hi raha — safety rule kaam kar raha\n");
    } else {
        cout << "(Elevator abhi ruka hua hai — Scene 4 me khulke dekhenge)\n";
    }

    // ========================================================================
    //  SCENE 4 — RUKI ELEVATOR KA DARWAZA (Requirement #11, #12)
    // ========================================================================
    scene("SCENE 4 — Ruki elevator ka darwaza khulta hai (Req #11, #12)");

    // Elevator ko IDLE hone tak chalao (saare stops nipta de).
    for (int i = 0; i < 12 && !e1->isIdle(); i++) {
        controller->runOneTick();
    }

    cout << "\nAb Elevator#1 ruka hua hai: ";
    e1->printState();
    cout << "\nOpen-door button dabate hain:\n";
    controller->pressOpenDoor(1);
    cout << (e1->isDoorOpen() ? "✅ Darwaza khul gaya (kyunki elevator ruka hai)\n" : "❌ Nahi khula?\n");

    // ========================================================================
    //  SCENE 5 — CAPACITY (Requirement #19, #20)
    //  "up to eight people or a total weight of 680 kilograms"
    // ========================================================================
    scene("SCENE 5 — ⭐ Capacity: 8 log YA 680 kg (Req #19, #20)");

    cout << "Darwaza khula hai, ab passengers chadhate hain (har ek 85 kg):\n\n";
    // 8 log chadhane ki koshish — 8 tak chalna chahiye, 9th reject.
    for (int i = 1; i <= 9; i++) {
        cout << "Passenger " << i << ": ";
        bool ok = controller->boardPassenger(1, 85);
        if (!ok && i == 9) {
            cout << "  ⬆ ✅ Sahi! 8 log ki limit lag gayi (Req #19)\n";
        }
    }

    cout << "\n--- Ab WEIGHT wali limit test karte hain ---\n";
    cout << "Sabko utaro, phir bhaari log chadhao:\n";
    for (int i = 0; i < 8; i++) {
        controller->exitPassenger(1, 85);
    }

    cout << "\nAb 5 bhaari log (150 kg each = 750 kg > 680 kg limit):\n";
    for (int i = 1; i <= 5; i++) {
        cout << "Bhaari passenger " << i << ": ";
        bool ok = controller->boardPassenger(1, 150);
        if (!ok) {
            cout << "  ⬆ ✅ Sahi! Log to sirf " << e1->getPeopleCount() << " hain (8 se kam), par WEIGHT limit paar\n";
            cout << "  📌 Yahi wajah hai ki DONO check zaroori hain — sirf log ginte to ye pakda hi nahi jaata\n";
            break;
        }
    }

    // ========================================================================
    //  SCENE 6 — DISPLAYS (Requirement #16, #17, #18)
    // ========================================================================
    scene("SCENE 6 — Displays: andar/bahar (Req #16), capacity (Req #17), floor (Req #18)");

    controller->showAllDisplays();
    cout << "\n📌 Notice: OUTSIDE display pe sirf floor+direction hai.\n";
    cout << "   INSIDE display pe capacity BHI hai — Requirement #17.\n";

    cout << "\n--- Floor displays (Req #18) ---\n";
    controller->showFloorDisplay(0);
    controller->showFloorDisplay(7);
    controller->showFloorDisplay(14);

    // ========================================================================
    //  SCENE 7 — INSIDE PANEL + MULTIPLE PASSENGERS (Requirement #14, #23)
    // ========================================================================
    scene("SCENE 7 — Inside panel (Req #14) + multiple passengers (Req #23)");

    cout << "Ek hi elevator me kai log, alag-alag floors ke liye:\n";
    controller->selectFloorInsideElevator({1, 10}); // koi 10th floor jaana chahta
    controller->selectFloorInsideElevator({1, 4});  // koi 4th
    controller->selectFloorInsideElevator({1, 14}); // koi top floor

    cout << "\n📌 Teeno request ek hi elevator me hain (Req #23). Elevator inhe\n";
    cout << "   apne raaste ke hisaab se nipta dega — har baar mudega nahi.\n";

    for (int i = 0; i < 6; i++) {
        controller->runOneTick();
    }

    // ========================================================================
    //  CLEANUP
    // ========================================================================
    scene("Demo khatam");
    delete controller;
    // ⭐ `delete controller` andar ke saare elevators bhi delete kar deta hai
    // (uske destructor me). Panels value-type hain, wo apne aap saaf ho jaate.
    // Isi liye yahan sirf EK delete hai.

    return 0;
}
