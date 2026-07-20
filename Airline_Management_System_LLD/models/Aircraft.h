// ============================================================================
//  models/Aircraft.h  —  Ek jahaaz (physical plane) + uski seat capacity
// ----------------------------------------------------------------------------
//  Aircraft batata hai ki is jahaaz me kitni seats hain, class-wise. Jab ye
//  aircraft kisi flight ko assign hota hai, tab in numbers ke hisaab se us
//  flight ki asli seats generate hoti hain (SeatInventoryService karta hai).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ AIRCRAFT aur FLIGHT ek cheez NAHI hain — farak samajhna zaroori       │
//  │                                                                          │
//  │     AIRCRAFT -> physical jahaaz (Airbus A320, 30+6+4 seats)              │
//  │     FLIGHT   -> ek trip (DEL->BOM, day 10, 9:00 AM)                       │
//  │                                                                          │
//  │  Ek hi aircraft (AC_320) alag-alag flights pe assign ho sakta hai (subah  │
//  │  DEL->BOM, shaam BOM->DEL). Isi liye seats FLIGHT ke saath banti hain,    │
//  │  aircraft ke saath nahi — aircraft to bas "template/capacity" deta hai.  │
//  │  (main.cpp me AC_320 do flights ko assign hota hai — yahi dikhata hai.)   │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_AIRCRAFT_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_AIRCRAFT_H

#include <string>

namespace airline_mgmt {

struct Aircraft {
    std::string aircraftId;   // "AC_320" — unique
    std::string model;        // "Airbus A320"
    int economySeats;         // kitni economy seats
    int premiumEconomySeats;  // kitni premium-economy
    int businessSeats;        // kitni business

    // Total capacity — teeno class ka jod (chhota helper).
    int totalSeats() const { return economySeats + premiumEconomySeats + businessSeats; }
};

} // namespace airline_mgmt

#endif
