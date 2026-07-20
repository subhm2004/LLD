// ============================================================================
//  main.cpp  —  Airline Management System ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Har scene ek ya do requirements ko CHALA ke dikhata hai — sirf likha nahi,
//  asli output me. Dhyaan do client kitna kam jaanta hai (Facade ka fayda):
//  poore system me 5 services + 1 strategy + 10 models hain, par yahan sirf
//  `AirlineManagementSystem` dikhta hai. Baaki sab andar chhupa hua.
//
//  ============================================================================
//   DEMO PLAN — kaunsa step kaunsi requirement dikhata hai
//  ----------------------------------------------------------------------------
//     Setup   -> #4,#5  Airports, users (roles), passenger, aircraft, crew
//     Admin   -> #3     Flight schedule (sirf ADMIN kar sakta hai — RBAC)
//     Staff   -> #3     Aircraft + crew assign (seats generate)
//     Search  -> #1     Source/dest/day se flight dhoondho
//     Booking -> #2     Seats select + pay (PENDING -> CONFIRMED)
//     Concurrency -> #8 Same seat dobara book -> reject (double-booking rok)
//     Change  -> #7     Confirmed booking ko dusri flight pe le jao
//     Cancel  -> #6     Booking cancel + refund (seats release)
//     Baggage -> #4     30 kg se zyada -> reject
//     Delay   -> #3     Staff flight delay karta hai
//  ============================================================================
#include <bits/stdc++.h>

#include "core/AirlineManagementSystem.h"

using namespace std;
using namespace airline_mgmt;

int main() {
    AirlineManagementSystem system;

    cout << "=== Airline Management System LLD ===\n\n";

    // ---- Airports add (infrastructure) -------------------------------------
    system.addAirport({"DEL", "Indira Gandhi", "Delhi"});
    system.addAirport({"BOM", "Chhatrapati Shivaji", "Mumbai"});
    system.addAirport({"BLR", "Kempegowda", "Bengaluru"});

    // ---- Users register (R5: teen roles) -----------------------------------
    system.registerUser({"U_ADMIN", "Admin User", "admin@airline.com", UserRole::ADMIN, ""});
    system.registerUser({"U_STAFF", "Staff User", "staff@airline.com", UserRole::AIRLINE_STAFF, ""});
    system.registerUser({"U_PAX", "Passenger User", "pax@email.com", UserRole::PASSENGER, "P1"});

    // ---- Passenger register (R4: personal details + baggage) ---------------
    system.registerPassenger({"P1", "Shubham Malik", "pax@email.com", "+91-9876543210", 18.5});

    // ---- Aircraft + crew add ------------------------------------------------
    system.addAircraft({"AC_320", "Airbus A320", 30, 6, 4}); // 30 eco + 6 premium + 4 business
    system.addCrewMember({"CR_P1", "Captain Rao", CrewRole::PILOT});
    system.addCrewMember({"CR_C1", "Neha Cabin", CrewRole::CABIN_CREW});

    // ---- ADMIN: flight schedule (R3) — sirf admin kar sakta hai ------------
    system.login("U_ADMIN");
    system.scheduleFlight({"AI-101", "DEL", "BOM", 10, 540, "", FlightStatus::SCHEDULED}); // 9:00 AM
    system.scheduleFlight({"AI-202", "DEL", "BLR", 10, 600, "", FlightStatus::SCHEDULED}); // 10:00 AM

    // ---- STAFF: aircraft + crew assign (R3) --------------------------------
    system.login("U_STAFF");
    system.assignAircraftToFlight("AI-101", "AC_320"); // seats generate ho jaati hain
    system.assignCrewToFlight("AI-101", "CR_P1");      // pilot
    system.assignCrewToFlight("AI-101", "CR_C1");      // cabin -> ab minimum crew poori

    system.assignAircraftToFlight("AI-202", "AC_320"); // dusri flight ko bhi wahi aircraft

    // ---- SEARCH (R1) --------------------------------------------------------
    cout << "\n--- Search DEL -> BOM day 10 ---\n";
    system.printSearchResults("DEL", "BOM", 10);

    cout << "\n--- Available seats AI-101 ---\n";
    system.printAvailableSeats("AI-101");

    vector<string> seatIds = {"AI-101-E1", "AI-101-E2"}; // 2 economy seats

    // ---- BOOKING + PAYMENT (R2) --------------------------------------------
    cout << "\n--- Booking + payment (passenger flow) ---\n";
    string bookingId = system.createBooking("P1", "AI-101", seatIds); // PENDING
    system.payForBooking(bookingId);                                  // CONFIRMED + notify

    // ---- CONCURRENCY (R8): wahi seat dobara book -> reject -----------------
    cout << "\n--- Concurrent seat grab demo (second booking same seat) ---\n";
    try {
        system.createBooking("P1", "AI-101", {"AI-101-E1"}); // E1 already BOOKED
        cout << "ERROR: double booking allowed!\n";
    } catch (const exception &ex) {
        cout << "Expected: " << ex.what() << "\n"; // "Seat not available"
    }

    // ---- FLIGHT CHANGE (R7): DEL->BOM booking ko DEL->BLR pe le jao --------
    cout << "\n--- Flight change DEL->BLR ---\n";
    system.assignCrewToFlight("AI-202", "CR_P1"); // AI-202 ko bhi crew do
    system.assignCrewToFlight("AI-202", "CR_C1");
    string newBooking = system.changeFlight(bookingId, "AI-202", {"AI-202-E1"});

    // ---- CANCEL + REFUND (R6) ----------------------------------------------
    cout << "\n--- Cancel new booking with refund ---\n";
    system.cancelBooking(newBooking); // CONFIRMED tha -> REFUNDED + seat release

    // ---- BAGGAGE LIMIT (R4): 30 kg se zyada -> reject ----------------------
    cout << "\n--- Baggage limit ---\n";
    system.registerPassenger({"P2", "Heavy Bag", "heavy@email.com", "+91-000", 35.0}); // 35 kg!
    try {
        system.createBooking("P2", "AI-202", {"AI-202-E2"});
        cout << "ERROR: overweight baggage allowed!\n";
    } catch (const exception &ex) {
        cout << "Expected: " << ex.what() << "\n"; // "Baggage exceeds airline limit"
    }

    // ---- STAFF: flight delay (R3) ------------------------------------------
    cout << "\n--- Staff: delay flight ---\n";
    system.delayFlight("AI-202", 630); // naya dep time 10:30 AM

    cout << "\nAirline_Management_System_LLD demo completed.\n";
    return 0;
}
