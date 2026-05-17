#ifndef OYO_HOTEL_BOOKING_LLD_SERVICES_AVAILABILITYSERVICE_H
#define OYO_HOTEL_BOOKING_LLD_SERVICES_AVAILABILITYSERVICE_H

#include <bits/stdc++.h>

#include "../enums/BookingStatus.h"
#include "../models/Booking.h"
#include "../models/Room.h"

namespace oyo_hotel_lld {

using namespace std;

class AvailabilityService {
public:
    static bool isDateRangeOverlapping(int checkInA, int checkOutA, int checkInB, int checkOutB) {
        return checkInA < checkOutB && checkInB < checkOutA;
    }

    static bool isRoomAvailableForStay(const Room &room,
                                       int checkInDay,
                                       int checkOutDay,
                                       const unordered_map<string, Booking> &bookings) {
        if (room.getStatus() == RoomStatus::MAINTENANCE) {
            return false;
        }

        for (const auto &entry : bookings) {
            const Booking &booking = entry.second;
            if (booking.getRoomId() != room.getRoomId()) {
                continue;
            }
            if (booking.getStatus() == BookingStatus::CANCELLED ||
                booking.getStatus() == BookingStatus::CHECKED_OUT) {
                continue;
            }
            if (isDateRangeOverlapping(checkInDay, checkOutDay, booking.getCheckInDay(),
                                     booking.getCheckOutDay())) {
                return false;
            }
        }
        return true;
    }

    static vector<Room *> filterAvailableRooms(const vector<Room *> &rooms,
                                               int checkInDay,
                                               int checkOutDay,
                                               const unordered_map<string, Booking> &bookings) {
        vector<Room *> available;
        for (Room *room : rooms) {
            if (room != nullptr &&
                isRoomAvailableForStay(*room, checkInDay, checkOutDay, bookings)) {
                available.push_back(room);
            }
        }
        return available;
    }
};

}  // namespace oyo_hotel_lld

#endif
