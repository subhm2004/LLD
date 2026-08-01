// ============================================================================
//  main.cpp  —  OYO Hotel Booking ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Ek poora flow chala ke dikhata hai:
//    1. Setup: guests, hotels, rooms register
//    2. Search: city ke hotels (rating-sorted)
//    3. Availability: date-range pe free rooms
//    4. Booking: room reserve + price + confirm + notify
//    5. Overlap reject: same room, overlapping dates -> fail
//    6. ⭐ Strategy swap: weekend pricing pe switch
//    7. Check-in / check-out lifecycle
//    8. Cancel
//
//  ⭐ Client ko sirf `OYOHotelBookingSystem` dikhta hai (Facade). Availability
//     math, pricing strategy, notifications — kuch nahi. Bas saaf methods.
//
//  📌 Include targeted (bits/stdc++.h nahi) — portable + saaf.
// ============================================================================
#include <exception>
#include <iostream>
#include <string>

#include "core/OYOHotelBookingSystem.h"

using namespace std;
using namespace oyo_hotel_lld;

int main() {
    OYOHotelBookingSystem system;

    // ---- Setup: guests + hotels + rooms ------------------------------------
    system.registerGuest(Guest("G1", "Shubham", "+91-9999999991", "shubham@email.com"));
    system.registerGuest(Guest("G2", "Ananya", "+91-9999999992", "ananya@email.com"));

    system.addHotel(Hotel("H1", "OYO Townhouse Jaipur", "Jaipur", "MI Road, Jaipur", 4.2));
    system.addHotel(Hotel("H2", "OYO Flagship Delhi", "Delhi", "Connaught Place", 4.0));

    system.addRoom(Room("R101", "H1", RoomType::STANDARD, 1200.0, 1));
    system.addRoom(Room("R102", "H1", RoomType::DELUXE, 1800.0, 2));
    system.addRoom(Room("R201", "H2", RoomType::STANDARD, 1500.0, 2));
    system.addRoom(Room("R202", "H2", RoomType::SUITE, 3500.0, 5));

    // ---- Search + availability ---------------------------------------------
    cout << "=== Search Jaipur ===\n";
    system.printHotelsInCity("Jaipur");

    cout << "\n=== Availability H1 (day 10-12) ===\n";
    system.printAvailableRooms("H1", 10, 12);

    // ---- Booking (standard pricing: 1800 x 2 = 3600) -----------------------
    string booking1 = system.createBooking("G1", "R102", 10, 12);

    // ---- Overlap reject: R102 already 10-12, ab 11-13 overlaps ------------
    cout << "\n=== Overlap attempt same room (should fail in real try-catch) ===\n";
    try {
        system.createBooking("G2", "R102", 11, 13); // overlaps [10,12)
        cout << "ERROR: overlap should have failed\n";
    } catch (const exception &ex) {
        cout << "Expected error: " << ex.what() << "\n";
    }

    // ---- ⭐ Strategy swap: weekend pricing (R101 days 12-15 = 4200) --------
    cout << "\n=== Weekend pricing demo ===\n";
    system.useWeekendPricing();
    string booking2 = system.createBooking("G2", "R101", 12, 15); // 1200 + 1500 + 1500

    // ---- Lifecycle: check-in -> check-out (room fir se free) ---------------
    system.checkIn(booking1);
    system.checkOut(booking1);

    cout << "\n=== After checkout — room available again ===\n";
    system.printAvailableRooms("H1", 10, 12); // R102 wapas available (booking checked out)

    // ---- Cancel booking2 ---------------------------------------------------
    cout << "\n=== Cancel booking2 ===\n";
    system.cancelBooking(booking2);

    cout << "\nOYO_Hotel_Booking_LLD demo completed.\n";
    return 0;
}
