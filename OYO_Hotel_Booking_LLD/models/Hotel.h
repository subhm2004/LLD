#ifndef OYO_HOTEL_BOOKING_LLD_MODELS_HOTEL_H
#define OYO_HOTEL_BOOKING_LLD_MODELS_HOTEL_H

#include <bits/stdc++.h>

namespace oyo_hotel_lld {

using namespace std;

class Hotel {
public:
    Hotel(string hotelId, string name, string city, string address, double rating)
        : hotelId_(std::move(hotelId)),
          name_(std::move(name)),
          city_(std::move(city)),
          address_(std::move(address)),
          rating_(rating) {}

    const string &getHotelId() const { return hotelId_; }
    const string &getName() const { return name_; }
    const string &getCity() const { return city_; }
    const string &getAddress() const { return address_; }
    double getRating() const { return rating_; }

private:
    string hotelId_;
    string name_;
    string city_;
    string address_;
    double rating_;
};

}  // namespace oyo_hotel_lld

#endif
