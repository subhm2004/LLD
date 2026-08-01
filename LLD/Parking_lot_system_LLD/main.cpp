// ============================================================================
//  main.cpp — PARKING LOT SYSTEM ka demo driver (saare requirements exercise)
// ----------------------------------------------------------------------------
//  Ye demo image wale SAARE 11 requirements dikhata hai:
//    setup: gates + 4-type spots + display board (observer)
//    entry: alag vehicles (car/truck/van/moto + handicapped) park karo
//    board: availability + full notification automatically update
//    exit:  ticket se fee (hourly) + payment (machine/agent + card/cash/upi)
//    full:  chhoti lot bhar ke MAX CAPACITY rule + full notification dikhao
//
//  DHYAN: capacity yahan CHHOTI rakhi hai (demo ke liye) taaki "full" scenario
//  dikh sake. Real requirement 40,000 hai — bas number badalna hai.
// ============================================================================
#include <bits/stdc++.h>

#include "core/ParkingLot.h"
#include "models/DisplayBoard.h"
#include "models/Vehicle.h"
#include "strategies/HourlyPricingStrategy.h"

using namespace std;
using namespace parking_lot_lld;

int main() {
    // ---- SETUP: lot banao (chhoti capacity=5 demo ke liye; real me 40000) ----
    ParkingLot *lot = new ParkingLot(new HourlyPricingStrategy(), /*maxCapacity=*/5);

    // Requirement #3 — multiple entry/exit gates
    lot->addEntryGate(1);
    lot->addEntryGate(2);
    lot->addExitGate(1);
    lot->addExitGate(2);

    // Requirement #2 — 4 spot types (handicapped/compact/large/motorcycle)
    lot->addSpot(101, SpotType::HANDICAPPED);
    lot->addSpot(102, SpotType::COMPACT);
    lot->addSpot(103, SpotType::COMPACT);
    lot->addSpot(104, SpotType::LARGE);
    lot->addSpot(105, SpotType::MOTORCYCLE);

    // Requirement #5 — display board (OBSERVER: lot pe subscribe)
    DisplayBoard board;
    lot->addObserver(&board);
    board.display();  // initial state — sab khaali

    // ---- ENTRY: alag vehicles park karo (requirement #4) ----
    cout << "\n----- Vehicles entering -----\n";
    Vehicle car("CAR-301", VehicleType::CAR);
    Vehicle truck("TRUCK-801", VehicleType::TRUCK);
    Vehicle moto("MOTO-201", VehicleType::MOTORCYCLE);
    Vehicle handiCar("CAR-999", VehicleType::CAR, /*handicappedPermit=*/true);

    string carTicket = lot->parkVehicle(car, 1);          // compact spot milega
    string truckTicket = lot->parkVehicle(truck, 2);      // large spot (truck ke liye)
    string motoTicket = lot->parkVehicle(moto, 1);        // motorcycle spot
    string handiTicket = lot->parkVehicle(handiCar, 2);   // handicapped spot (permit hai)

    board.display();  // ab availability kam dikhegi

    // ---- FULL SCENARIO: last spot bharo -> MAX CAPACITY (req #6/#7) ----
    cout << "\n----- One more car (fills the lot) -----\n";
    Vehicle car2("CAR-302", VehicleType::CAR);
    string car2Ticket = lot->parkVehicle(car2, 1);  // last compact spot -> FULL!

    board.display();  // STATUS: PARKING FULL

    // Ab ek aur vehicle try kare -> reject (max capacity rule)
    cout << "\n----- Trying to park when FULL -----\n";
    try {
        Vehicle lateCar("CAR-500", VehicleType::CAR);
        lot->parkVehicle(lateCar, 2);
    } catch (const exception &e) {
        cout << "Rejected: " << e.what() << "\n";
    }

    // ---- EXIT: fee + payment (requirement #9/#10/#11) ----
    cout << "\n----- Vehicles exiting (payment) -----\n";
    // Car — automated machine pe credit card se
    lot->unparkVehicle(carTicket, 1, PaymentChannel::AUTOMATED_MACHINE, PaymentMethod::CREDIT_CARD);
    // Truck — agent ko cash se
    lot->unparkVehicle(truckTicket, 2, PaymentChannel::AGENT, PaymentMethod::CASH);
    // Motorcycle — machine pe UPI se
    lot->unparkVehicle(motoTicket, 1, PaymentChannel::AUTOMATED_MACHINE, PaymentMethod::UPI);

    board.display();  // spots wapas khaali -> lot ab OPEN

    // Baaki cleanup exit
    lot->unparkVehicle(handiTicket, 2, PaymentChannel::AGENT, PaymentMethod::DEBIT_CARD);
    lot->unparkVehicle(car2Ticket, 1, PaymentChannel::AUTOMATED_MACHINE, PaymentMethod::CASH);

    board.display();  // sab khaali wapas

    delete lot;
    return 0;
}
