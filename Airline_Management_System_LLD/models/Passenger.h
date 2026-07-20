// ============================================================================
//  models/Passenger.h  —  Ek yaatri (jiske naam ticket hai)
// ----------------------------------------------------------------------------
//  Passenger ki personal details + baggage weight. Booking passenger se bandhi
//  hoti hai (kiske naam booking hai).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ PASSENGER aur USER alag kyun hain?                                    │
//  │                                                                          │
//  │     USER      -> jo system me login karta hai (role: admin/staff/pax)     │
//  │     PASSENGER -> jiske naam par ticket banti hai (baggage, contact)       │
//  │                                                                          │
//  │  Ek user (jaise koi papa) apne poore parivaar ke liye ticket book kar     │
//  │  sakta hai — tab ek USER ke neeche kai PASSENGER honge. Isi liye User me  │
//  │  `linkedPassengerId` hota hai jo user ko uske passenger record se jodta.  │
//  │  Real duniya me ye distinction zaroori hai; isi liye alag models.        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ `baggageKg` pe rule hai: max 30 kg (business rule). Facade ka
//     validateBaggage() booking se pehle check karta hai — 30 se zyada to
//     "Baggage exceeds airline limit" throw. Aur negative baggage register hi
//     nahi hota (registerPassenger me guard hai).
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_PASSENGER_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_PASSENGER_H

#include <string>

namespace airline_mgmt {

struct Passenger {
    std::string passengerId; // "P1" — unique
    std::string name;        // "Shubham Malik"
    std::string email;       // notifications yahan jaati hain
    std::string phone;       // contact number
    double baggageKg;        // saamaan ka weight (max 30 kg — business rule)
};

} // namespace airline_mgmt

#endif
