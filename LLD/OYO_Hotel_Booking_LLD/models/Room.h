// ============================================================================
//  models/Room.h  —  Ek room (bookable unit) ek hotel ke andar
// ----------------------------------------------------------------------------
//  Har room kisi ek hotel ka hissa (hotelId se bandha). Isme uska type, per-night
//  price, floor, aur status hota hai.
//
//  📌 Room ek "rich model" hai (data + thoda behavior: setStatus). `status_`
//     AVAILABLE se shuru hota. ⚠ Par yaad rahe (RoomStatus.h note): availability
//     ka ASLI faisla date-overlap se hota hai, is status se nahi — status sirf
//     coarse flag hai (MAINTENANCE hi availability block karta).
//
//  💡 Price room me store hai (type me nahi) — kyunki same type ke do rooms ka
//     daam alag ho sakta (floor/view ke hisaab se). Room ka apna explicit price.
// ============================================================================
#ifndef OYO_HOTEL_BOOKING_LLD_MODELS_ROOM_H
#define OYO_HOTEL_BOOKING_LLD_MODELS_ROOM_H

#include <string>
#include <utility>

#include "../enums/RoomStatus.h"
#include "../enums/RoomType.h"

namespace oyo_hotel_lld {

using namespace std;

class Room {
public:
    Room(string roomId, string hotelId, RoomType type, double pricePerNight, int floor)
        : roomId_(std::move(roomId)),
          hotelId_(std::move(hotelId)),
          type_(type),
          pricePerNight_(pricePerNight),
          floor_(floor),
          status_(RoomStatus::AVAILABLE) {} // naya room hamesha AVAILABLE se

    const string &getRoomId() const { return roomId_; }
    const string &getHotelId() const { return hotelId_; } // kis hotel ka
    RoomType getType() const { return type_; }
    double getPricePerNight() const { return pricePerNight_; }
    int getFloor() const { return floor_; }
    RoomStatus getStatus() const { return status_; }

    void setStatus(RoomStatus status) { status_ = status; } // coarse flag update

private:
    string roomId_;        // "R101" — unique
    string hotelId_;       // parent hotel
    RoomType type_;        // STANDARD/DELUXE/SUITE
    double pricePerNight_; // per-night base price (pricing strategy isko use karti)
    int floor_;
    RoomStatus status_;    // coarse flag (availability ka authority nahi — RoomStatus.h note)
};

}  // namespace oyo_hotel_lld

#endif
