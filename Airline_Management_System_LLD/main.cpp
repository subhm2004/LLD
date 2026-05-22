#include <bits/stdc++.h>

#include "core/AirlineManagementSystem.h"

using namespace std;
using namespace airline_mgmt;

int main() {
    AirlineManagementSystem system;

    cout << "=== Airline Management System LLD ===\n\n";

    system.addAirport({"DEL", "Indira Gandhi", "Delhi"});
    system.addAirport({"BOM", "Chhatrapati Shivaji", "Mumbai"});
    system.addAirport({"BLR", "Kempegowda", "Bengaluru"});

    system.registerUser({"U_ADMIN", "Admin User", "admin@airline.com", UserRole::ADMIN, ""});
    system.registerUser({"U_STAFF", "Staff User", "staff@airline.com", UserRole::AIRLINE_STAFF, ""});
    system.registerUser({"U_PAX", "Passenger User", "pax@email.com", UserRole::PASSENGER, "P1"});

    system.registerPassenger({"P1", "Shubham Malik", "pax@email.com", "+91-9876543210", 18.5});

    system.addAircraft({"AC_320", "Airbus A320", 30, 6, 4});
    system.addCrewMember({"CR_P1", "Captain Rao", CrewRole::PILOT});
    system.addCrewMember({"CR_C1", "Neha Cabin", CrewRole::CABIN_CREW});

    system.login("U_ADMIN");
    system.scheduleFlight({"AI-101", "DEL", "BOM", 10, 540, "", FlightStatus::SCHEDULED});
    system.scheduleFlight({"AI-202", "DEL", "BLR", 10, 600, "", FlightStatus::SCHEDULED});

    system.login("U_STAFF");
    system.assignAircraftToFlight("AI-101", "AC_320");
    system.assignCrewToFlight("AI-101", "CR_P1");
    system.assignCrewToFlight("AI-101", "CR_C1");

    system.assignAircraftToFlight("AI-202", "AC_320");

    cout << "\n--- Search DEL -> BOM day 10 ---\n";
    system.printSearchResults("DEL", "BOM", 10);

    cout << "\n--- Available seats AI-101 ---\n";
    system.printAvailableSeats("AI-101");

    vector<string> seatIds = {"AI-101-E1", "AI-101-E2"};

    cout << "\n--- Booking + payment (passenger flow) ---\n";
    string bookingId = system.createBooking("P1", "AI-101", seatIds);
    system.payForBooking(bookingId);

    cout << "\n--- Concurrent seat grab demo (second booking same seat) ---\n";
    try {
        system.createBooking("P1", "AI-101", {"AI-101-E1"});
    } catch (const exception &ex) {
        cout << "Expected: " << ex.what() << "\n";
    }

    cout << "\n--- Flight change DEL->BLR ---\n";
    system.assignCrewToFlight("AI-202", "CR_P1");
    system.assignCrewToFlight("AI-202", "CR_C1");
    string newBooking = system.changeFlight(bookingId, "AI-202", {"AI-202-E1"});

    cout << "\n--- Cancel new booking with refund ---\n";
    system.cancelBooking(newBooking);

    cout << "\n--- Baggage limit ---\n";
    system.registerPassenger({"P2", "Heavy Bag", "heavy@email.com", "+91-000", 35.0});
    try {
        system.createBooking("P2", "AI-202", {"AI-202-E2"});
    } catch (const exception &ex) {
        cout << "Expected: " << ex.what() << "\n";
    }

    cout << "\n--- Staff: delay flight ---\n";
    system.delayFlight("AI-202", 630);

    cout << "\nAirline_Management_System_LLD demo completed.\n";
    return 0;
}
