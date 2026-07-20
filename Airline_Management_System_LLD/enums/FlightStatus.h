// ============================================================================
//  enums/FlightStatus.h  —  Ek flight abhi kis haal me hai
// ----------------------------------------------------------------------------
//  Ek flight apni zindagi me in states me se kisi ek me hoti hai:
//
//     SCHEDULED -> plan ho gayi, booking khul sakti hai (bookable state)
//     DELAYED   -> late ho gayi (departure time badal gaya) — staff set karta hai
//     CANCELLED -> flight hi cancel ho gayi
//     DEPARTED  -> ud chuki hai, ab booking possible nahi
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SIRF `SCHEDULED` flight hi book ho sakti hai                          │
//  │                                                                          │
//  │  - Search sirf SCHEDULED flights laata hai (FlightSearchService)         │
//  │  - createBooking() pehle check karta hai: status == SCHEDULED?           │
//  │    Nahi to "Flight not bookable" throw. DELAYED/CANCELLED/DEPARTED pe     │
//  │    naya booking nahi khulta.                                             │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_ENUMS_FLIGHTSTATUS_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_ENUMS_FLIGHTSTATUS_H

namespace airline_mgmt {

enum class FlightStatus { SCHEDULED, DELAYED, CANCELLED, DEPARTED };

} // namespace airline_mgmt

#endif
