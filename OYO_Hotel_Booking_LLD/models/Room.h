#ifndef OYO_HOTEL_BOOKING_LLD_MODELS_ROOM_H
#define OYO_HOTEL_BOOKING_LLD_MODELS_ROOM_H

#include <bits/stdc++.h>

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
          status_(RoomStatus::AVAILABLE) {}

    const string &getRoomId() const { return roomId_; }
    const string &getHotelId() const { return hotelId_; }
    RoomType getType() const { return type_; }
    double getPricePerNight() const { return pricePerNight_; }
    int getFloor() const { return floor_; }
    RoomStatus getStatus() const { return status_; }

    void setStatus(RoomStatus status) { status_ = status; }

private:
    string roomId_;
    string hotelId_;
    RoomType type_;
    double pricePerNight_;
    int floor_;
    RoomStatus status_;
};

}  // namespace oyo_hotel_lld

#endif
