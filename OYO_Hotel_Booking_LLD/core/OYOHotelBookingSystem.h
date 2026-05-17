#ifndef OYO_HOTEL_BOOKING_LLD_CORE_OYOHOTELBOOKINGSYSTEM_H
#define OYO_HOTEL_BOOKING_LLD_CORE_OYOHOTELBOOKINGSYSTEM_H

#include <bits/stdc++.h>

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
    void registerGuest(const Guest &guest) { upsert(guests_, guest.getGuestId(), guest); }

    void addHotel(const Hotel &hotel) { upsert(hotels_, hotel.getHotelId(), hotel); }

    void addRoom(const Room &room) {
        if (!hotels_.count(room.getHotelId())) {
            throw runtime_error("Hotel not found: " + room.getHotelId());
        }
        upsert(rooms_, room.getRoomId(), room);
    }

    vector<Hotel *> searchHotelsByCity(const string &city) const {
        vector<Hotel *> result;
        for (const auto &entry : hotels_) {
            if (entry.second.getCity() == city) {
                result.push_back(const_cast<Hotel *>(&entry.second));
            }
        }
        sort(result.begin(), result.end(), [](const Hotel *a, const Hotel *b) {
            return a->getRating() > b->getRating();
        });
        return result;
    }

    vector<Room *> getAvailableRooms(const string &hotelId, int checkInDay, int checkOutDay) const {
        validateDateRange(checkInDay, checkOutDay);
        vector<Room *> hotelRooms;
        for (const auto &entry : rooms_) {
            if (entry.second.getHotelId() == hotelId) {
                hotelRooms.push_back(const_cast<Room *>(&entry.second));
            }
        }
        return AvailabilityService::filterAvailableRooms(hotelRooms, checkInDay, checkOutDay, bookings_);
    }

    string createBooking(const string &guestId,
                         const string &roomId,
                         int checkInDay,
                         int checkOutDay) {
        validateDateRange(checkInDay, checkOutDay);
        Guest *guest = getGuestOrThrow(guestId);
        Room *room = getRoomOrThrow(roomId);
        getHotelOrThrow(room->getHotelId());

        if (!AvailabilityService::isRoomAvailableForStay(*room, checkInDay, checkOutDay, bookings_)) {
            throw runtime_error("Room not available for selected dates");
        }

        const double amount =
            pricingService_.calculateStayAmount(room->getPricePerNight(), checkInDay, checkOutDay);
        const string bookingId = "OYO_" + to_string(++bookingCounter_);

        Booking booking(bookingId, guest->getGuestId(), room->getHotelId(), room->getRoomId(), checkInDay,
                        checkOutDay, amount);
        booking.setStatus(BookingStatus::CONFIRMED);
        upsert(bookings_, bookingId, booking);
        room->setStatus(RoomStatus::BOOKED);

        notificationService_.sendBookingConfirmed(guest->getEmail(), bookingId, amount);
        cout << "Booking confirmed: " << bookingId << " | Guest: " << guest->getName() << " | Room: "
             << room->getRoomId() << " | Nights: " << (checkOutDay - checkInDay) << " | Amount: Rs "
             << amount << "\n";
        return bookingId;
    }

    void cancelBooking(const string &bookingId) {
        Booking *booking = getBookingOrThrow(bookingId);
        if (booking->getStatus() == BookingStatus::CANCELLED) {
            throw runtime_error("Booking already cancelled");
        }
        if (booking->getStatus() == BookingStatus::CHECKED_IN) {
            throw runtime_error("Cannot cancel after check-in");
        }

        booking->setStatus(BookingStatus::CANCELLED);
        Room *room = getRoomOrThrow(booking->getRoomId());
        room->setStatus(RoomStatus::AVAILABLE);

        Guest *guest = getGuestOrThrow(booking->getGuestId());
        notificationService_.sendBookingCancelled(guest->getEmail(), bookingId);
        cout << "Booking cancelled: " << bookingId << "\n";
    }

    void checkIn(const string &bookingId) {
        Booking *booking = getBookingOrThrow(bookingId);
        if (booking->getStatus() != BookingStatus::CONFIRMED) {
            throw runtime_error("Only confirmed bookings can check in");
        }
        booking->setStatus(BookingStatus::CHECKED_IN);
        cout << "Check-in done: " << bookingId << "\n";
    }

    void checkOut(const string &bookingId) {
        Booking *booking = getBookingOrThrow(bookingId);
        if (booking->getStatus() != BookingStatus::CHECKED_IN) {
            throw runtime_error("Guest must check in before check out");
        }
        booking->setStatus(BookingStatus::CHECKED_OUT);
        Room *room = getRoomOrThrow(booking->getRoomId());
        room->setStatus(RoomStatus::AVAILABLE);
        cout << "Check-out done: " << bookingId << "\n";
    }

    void useWeekendPricing() {
        pricingService_.setStrategy(make_unique<WeekendSurchargePricingStrategy>());
        cout << "Pricing strategy switched to Weekend surcharge (1.25x on Sat/Sun)\n";
    }

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
            cout << room->getRoomId() << " | " << roomTypeToString(room->getType())
                 << " | Rs " << room->getPricePerNight() << "/night | Floor " << room->getFloor() << "\n";
        }
    }

private:
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

    unordered_map<string, Guest> guests_;
    unordered_map<string, Hotel> hotels_;
    unordered_map<string, Room> rooms_;
    unordered_map<string, Booking> bookings_;
    PricingService pricingService_;
    NotificationService notificationService_;
    int bookingCounter_{0};
};

}  // namespace oyo_hotel_lld

#endif
