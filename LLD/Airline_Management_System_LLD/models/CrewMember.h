// ============================================================================
//  models/CrewMember.h  —  Ek crew member (pilot / co-pilot / cabin crew)
// ----------------------------------------------------------------------------
//  Flight chalane wale staff. Har crew member ka ek role hota hai (CrewRole).
//  Flights ko crew assign kiya jaata hai (CrewAssignmentService), aur booking
//  tabhi khulti hai jab flight ke paas minimum crew ho (cockpit + cabin).
//
//  📌 Ek crew member alag-alag flights pe assign ho sakta hai — isi liye crew
//     ka data yahan alag rakha hai, aur "kaunsa crew kaunsi flight pe" wo mapping
//     facade ke `flightCrew_` (flightId -> [crewId...]) me hoti hai, model me nahi.
//     Ye SRP hai: CrewMember sirf "kaun hai" jaanta hai, "kahan laga hai" nahi.
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_CREWMEMBER_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_CREWMEMBER_H

#include <string>

#include "../enums/CrewRole.h"

namespace airline_mgmt {

struct CrewMember {
    std::string crewId; // "CR_P1" — unique
    std::string name;   // "Captain Rao"
    CrewRole role;      // PILOT / CO_PILOT / CABIN_CREW
};

} // namespace airline_mgmt

#endif
