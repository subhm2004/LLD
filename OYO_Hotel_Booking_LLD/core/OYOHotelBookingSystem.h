// ============================================================================
//  core/OYOHotelBookingSystem.h  —  Poore system ka FACADE (single entry point)
// ----------------------------------------------------------------------------
//  Ye class hi client (main.cpp) ke liye "OYO" hai. Andar 3 services + strategy +
//  4 data stores ka poora tamasha chalta hai, par bahar se client ko sirf saaf
//  methods dikhte hain: registerGuest, addHotel/addRoom, search, createBooking,
//  cancel, checkIn/checkOut.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FACADE PATTERN — subsystem ki complexity client se chhupao           │
//  │                                                                          │
//  │  Client ko ye NAHI pata:                                                 │
//  │    - hotels rating se sort kaise hote (search)                           │
//  │    - room availability date-overlap se kaise nikalti (AvailabilityService)│
//  │    - price kaise banta (PricingService + Strategy)                       │
//  │    - notification kaise jaata (NotificationService)                      │
//  │  Wo bas facade ka method bulata hai; facade sahi service ko delegate.    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ FACADE data ka MAALIK hai (guests/hotels/rooms/bookings — unordered_maps)
//     aur services ko orchestrate karta hai. Sab in-memory (interview-scope).
//     `PricingService` me unique_ptr member ki wajah se ye system non-copyable.
//
//  📌 BOOKING STATE MACHINE yahan enforce hota hai (BookingStatus.h ke rules):
//     checkIn sirf CONFIRMED se, checkOut sirf CHECKED_IN se, cancel check-in ke
//     baad nahi. Galat transition -> exception. Yahi invalid workflow rokta hai.
// ============================================================================
#ifndef OYO_HOTEL_BOOKING_LLD_CORE_OYOHOTELBOOKINGSYSTEM_H
#define OYO_HOTEL_BOOKING_LLD_CORE_OYOHOTELBOOKINGSYSTEM_H

#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/BookingStatus.h"
#include "../enums/RoomType.h"
#include "../models/Booking.h"
#include "../models/Guest.h"
#include "../models/Hotel.h"
#include "../models/Room.h"
#include "../services/AvailabilityService.h"
#include "../services/NotificationService.h"
#include "../services/PricingService.h"
#include "../strategies/WeekendSurchargePricingStrategy.h"

namespace oyo_hotel_lld {

using namespace std;

class OYOHotelBookingSystem {
public:
    // ---- Setup: guest / hotel / room register --------------------------------
    void registerGuest(const Guest &guest) { upsert(guests_, guest.getGuestId(), guest); }
    void addHotel(const Hotel &hotel) { upsert(hotels_, hotel.getHotelId(), hotel); }

    // Room add — par uska hotel exist karna chahiye (orphan room reject).
    void addRoom(const Room &room) {
        if (!hotels_.count(room.getHotelId())) {
            throw runtime_error("Hotel not found: " + room.getHotelId());
        }
        upsert(rooms_, room.getRoomId(), room);
    }

    // ---- Search: city ke hotels, rating ke hisaab se best-first ------------
    vector<Hotel *> searchHotelsByCity(const string &city) const {
        vector<Hotel *> result;
        for (const auto &entry : hotels_) {
            if (entry.second.getCity() == city) {
                // ⚠ const_cast: ye const method hai par mutable Hotel* deta hai.
                // Design wart hai (const method internals ko mutable de raha), par
                // safe kyunki underlying Hotel truly const nahi (bas const `this`
                // se accessed) aur caller ise mutate nahi karta. Cleaner hota
                // `const Hotel*` return karna.
                result.push_back(const_cast<Hotel *>(&entry.second));
            }
        }
        // Rating DESCENDING -> best hotel pehle.
        sort(result.begin(), result.end(),
             [](const Hotel *a, const Hotel *b) { return a->getRating() > b->getRating(); });
        return result;
    }

    // ---- Availability: hotel ke rooms jo is date-range me free hain ---------
    vector<Room *> getAvailableRooms(const string &hotelId, int checkInDay, int checkOutDay) const {
        validateDateRange(checkInDay, checkOutDay);
        vector<Room *> hotelRooms;
        for (const auto &entry : rooms_) {
            if (entry.second.getHotelId() == hotelId) {
                hotelRooms.push_back(const_cast<Room *>(&entry.second)); // (const_cast note upar)
            }
        }
        return AvailabilityService::filterAvailableRooms(hotelRooms, checkInDay, checkOutDay, bookings_);
    }

    // ========================================================================
    //  ⭐ CREATE BOOKING — validate -> availability -> price -> confirm -> notify
    // ========================================================================
    string createBooking(const string &guestId, const string &roomId, int checkInDay, int checkOutDay) {
        validateDateRange(checkInDay, checkOutDay);
        Guest *guest = getGuestOrThrow(guestId);
        Room *room = getRoomOrThrow(roomId);
        getHotelOrThrow(room->getHotelId()); // hotel exist verify

        // Gate: room is date-range ke liye available hai? (date-overlap check)
        if (!AvailabilityService::isRoomAvailableForStay(*room, checkInDay, checkOutDay, bookings_)) {
            throw runtime_error("Room not available for selected dates");
        }

        // Price nikaalo (current pricing strategy se — flat ya weekend surge).
        const double amount =
            pricingService_.calculateStayAmount(room->getPricePerNight(), checkInDay, checkOutDay);
        const string bookingId = "OYO_" + to_string(++bookingCounter_);

        Booking booking(bookingId, guest->getGuestId(), room->getHotelId(), room->getRoomId(),
                        checkInDay, checkOutDay, amount);
        booking.setStatus(BookingStatus::CONFIRMED); // is demo me turant confirmed
        upsert(bookings_, bookingId, booking);
        room->setStatus(RoomStatus::BOOKED); // coarse flag (availability ka authority nahi)

        // R10-style: guest ko confirmation notification.
        notificationService_.sendBookingConfirmed(guest->getEmail(), bookingId, amount);
        cout << "Booking confirmed: " << bookingId << " | Guest: " << guest->getName() << " | Room: "
             << room->getRoomId() << " | Nights: " << (checkOutDay - checkInDay) << " | Amount: Rs "
             << amount << "\n";
        return bookingId;
    }

    // ---- Cancel — sirf check-in se PEHLE (state machine guard) --------------
    void cancelBooking(const string &bookingId) {
        Booking *booking = getBookingOrThrow(bookingId);
        if (booking->getStatus() == BookingStatus::CANCELLED) {
            throw runtime_error("Booking already cancelled");
        }
        if (booking->getStatus() == BookingStatus::CHECKED_IN) {
            throw runtime_error("Cannot cancel after check-in"); // guest aa chuka
        }

        booking->setStatus(BookingStatus::CANCELLED);
        Room *room = getRoomOrThrow(booking->getRoomId());
        room->setStatus(RoomStatus::AVAILABLE); // coarse flag reset

        Guest *guest = getGuestOrThrow(booking->getGuestId());
        notificationService_.sendBookingCancelled(guest->getEmail(), bookingId);
        cout << "Booking cancelled: " << bookingId << "\n";
    }

    // ---- Check-in — sirf CONFIRMED se --------------------------------------
    void checkIn(const string &bookingId) {
        Booking *booking = getBookingOrThrow(bookingId);
        if (booking->getStatus() != BookingStatus::CONFIRMED) {
            throw runtime_error("Only confirmed bookings can check in");
        }
        booking->setStatus(BookingStatus::CHECKED_IN);
        cout << "Check-in done: " << bookingId << "\n";
    }

    // ---- Check-out — sirf CHECKED_IN se; room fir se free ------------------
    void checkOut(const string &bookingId) {
        Booking *booking = getBookingOrThrow(bookingId);
        if (booking->getStatus() != BookingStatus::CHECKED_IN) {
            throw runtime_error("Guest must check in before check out");
        }
        booking->setStatus(BookingStatus::CHECKED_OUT); // ab ye booking availability block nahi karti
        Room *room = getRoomOrThrow(booking->getRoomId());
        room->setStatus(RoomStatus::AVAILABLE);
        cout << "Check-out done: " << bookingId << "\n";
    }

    // ---- ⭐ Runtime pe weekend pricing pe switch (Strategy swap) ------------
    void useWeekendPricing() {
        pricingService_.setStrategy(make_unique<WeekendSurchargePricingStrategy>());
        cout << "Pricing strategy switched to Weekend surcharge (1.25x on Sat/Sun)\n";
    }

    // ---- Display helpers ---------------------------------------------------
    void printHotelsInCity(const string &city) const {
        cout << "\n=== Hotels in " << city << " ===\n";
        for (Hotel *hotel : searchHotelsByCity(city)) {
            cout << hotel->getHotelId() << " | " << hotel->getName() << " | Rating: " << hotel->getRating()
                 << " | " << hotel->getAddress() << "\n";
        }
    }

    void printAvailableRooms(const string &hotelId, int checkInDay, int checkOutDay) const {
        cout << "\n=== Available rooms (" << hotelId << ", day " << checkInDay << " to " << checkOutDay
             << ") ===\n";
        for (Room *room : getAvailableRooms(hotelId, checkInDay, checkOutDay)) {
            cout << room->getRoomId() << " | " << roomTypeToString(room->getType()) << " | Rs "
                 << room->getPricePerNight() << "/night | Floor " << room->getFloor() << "\n";
        }
    }

private:
    // Insert-or-update helper (key hai to update, warna emplace).
    template <typename Key, typename Value>
    static void upsert(unordered_map<Key, Value> &storage, const Key &key, const Value &value) {
        auto iterator = storage.find(key);
        if (iterator != storage.end()) {
            iterator->second = value;
        } else {
            storage.emplace(key, value);
        }
    }

    static void validateDateRange(int checkInDay, int checkOutDay) {
        if (checkOutDay <= checkInDay) {
            throw invalid_argument("checkOutDay must be after checkInDay");
        }
    }

    // ---- "get-or-throw" helpers — na mile to saaf error (fail-fast) --------
    Guest *getGuestOrThrow(const string &guestId) {
        auto it = guests_.find(guestId);
        if (it == guests_.end()) {
            throw runtime_error("Guest not found: " + guestId);
        }
        return &it->second;
    }
    Hotel *getHotelOrThrow(const string &hotelId) {
        auto it = hotels_.find(hotelId);
        if (it == hotels_.end()) {
            throw runtime_error("Hotel not found: " + hotelId);
        }
        return &it->second;
    }
    Room *getRoomOrThrow(const string &roomId) {
        auto it = rooms_.find(roomId);
        if (it == rooms_.end()) {
            throw runtime_error("Room not found: " + roomId);
        }
        return &it->second;
    }
    Booking *getBookingOrThrow(const string &bookingId) {
        auto it = bookings_.find(bookingId);
        if (it == bookings_.end()) {
            throw runtime_error("Booking not found: " + bookingId);
        }
        return &it->second;
    }

    // ---- Data (facade single source of truth — in-memory) ------------------
    unordered_map<string, Guest> guests_;
    unordered_map<string, Hotel> hotels_;
    unordered_map<string, Room> rooms_;
    unordered_map<string, Booking> bookings_;
    PricingService pricingService_;             // Strategy context (swappable pricing)
    NotificationService notificationService_;   // email/SMS
    int bookingCounter_{0};                     // unique booking id ("OYO_1"...)
};

}  // namespace oyo_hotel_lld

#endif
