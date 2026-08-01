// ============================================================================
//  models/Flight.h  —  Ek flight (ek trip: source se destination)
// ----------------------------------------------------------------------------
//  Ek flight = ek nirdharit trip. Kahan se kahan (source/destination airport
//  codes), kis din aur kitne baje (day + time), kaunsa aircraft laga hai, aur
//  abhi kis status me hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ TIME "minutes since midnight" me hai — string/clock nahi              │
//  │                                                                          │
//  │     departureTimeMinutes = 540  ->  9 * 60  ->  9:00 AM                   │
//  │     departureTimeMinutes = 600  ->  10:00 AM                              │
//  │                                                                          │
//  │  Ek plain int rakhna date/time class se bahut simple hai — compare karna  │
//  │  (kaunsi flight pehle), delay karna (naya number set) sab aasaan. Real    │
//  │  system me proper timestamp hota, par LLD demo ke liye int perfect hai.  │
//  │                                                                          │
//  │  📌 `aircraftId` shuru me KHAALI ("") hota hai — flight schedule karte    │
//  │     waqt aircraft assign nahi hoti. Baad me staff assignAircraftToFlight  │
//  │     karta hai. createBooking() check karta hai: aircraftId khaali to      │
//  │     booking nahi (seats generate hi nahi hui).                           │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_FLIGHT_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_FLIGHT_H

#include <string>

#include "../enums/FlightStatus.h"

namespace airline_mgmt {

struct Flight {
    std::string flightId;            // "AI-101" — unique
    std::string sourceAirport;       // "DEL" (Airport code)
    std::string destinationAirport;  // "BOM"
    int departureDay;                // kaunsa din (simple int, jaise 10)
    int departureTimeMinutes;        // midnight se minutes (540 = 9:00 AM)
    std::string aircraftId;          // laga hua aircraft ("" = abhi assign nahi)
    FlightStatus status;             // SCHEDULED / DELAYED / CANCELLED / DEPARTED
};

} // namespace airline_mgmt

#endif
