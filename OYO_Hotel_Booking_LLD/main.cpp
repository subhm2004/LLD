#include <bits/stdc++.h>

#include "core/OYOHotelBookingSystem.h"

using namespace std;
using namespace oyo_hotel_lld;

int main() {
    OYOHotelBookingSystem system;

    system.registerGuest(Guest("G1", "Shubham", "+91-9999999991", "shubham@email.com"));
    system.registerGuest(Guest("G2", "Ananya", "+91-9999999992", "ananya@email.com"));

    system.addHotel(Hotel("H1", "OYO Townhouse Jaipur", "Jaipur", "MI Road, Jaipur", 4.2));
    system.addHotel(Hotel("H2", "OYO Flagship Delhi", "Delhi", "Connaught Place", 4.0));

    system.addRoom(Room("R101", "H1", RoomType::STANDARD, 1200.0, 1));
    system.addRoom(Room("R102", "H1", RoomType::DELUXE, 1800.0, 2));
    system.addRoom(Room("R201", "H2", RoomType::STANDARD, 1500.0, 2));
    system.addRoom(Room("R202", "H2", RoomType::SUITE, 3500.0, 5));

    cout << "=== Search Jaipur ===\n";
    system.printHotelsInCity("Jaipur");

    cout << "\n=== Availability H1 (day 10-12) ===\n";
    system.printAvailableRooms("H1", 10, 12);

    string booking1 = system.createBooking("G1", "R102", 10, 12);

    cout << "\n=== Overlap attempt same room (should fail in real try-catch) ===\n";
    try {
        system.createBooking("G2", "R102", 11, 13);
    } catch (const exception &ex) {
        cout << "Expected error: " << ex.what() << "\n";
    }

    cout << "\n=== Weekend pricing demo ===\n";
    system.useWeekendPricing();
    string booking2 = system.createBooking("G2", "R101", 12, 15);

    system.checkIn(booking1);
    system.checkOut(booking1);

    cout << "\n=== After checkout — room available again ===\n";
    system.printAvailableRooms("H1", 10, 12);

    cout << "\n=== Cancel booking2 ===\n";
    system.cancelBooking(booking2);

    cout << "\nOYO_Hotel_Booking_LLD demo completed.\n";
    return 0;
}
