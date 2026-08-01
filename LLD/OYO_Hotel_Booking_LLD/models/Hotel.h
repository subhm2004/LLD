// ============================================================================
//  models/Hotel.h  —  Ek hotel (property)
// ----------------------------------------------------------------------------
//  Hotel ki details: id, naam, city, address, rating. Rooms is hotel ke andar
//  hote hain (Room me hotelId se linked — hotel khud rooms ki list nahi rakhta,
//  system unhe hotelId se filter karta).
//
//  ⭐ `rating` search me kaam aata: searchHotelsByCity rooms ko rating ke hisaab
//     se DESCENDING sort karta hai (best hotel pehle). Isi liye rating store hai.
//
//  📌 Hotel apni rooms ki list NAHI rakhta (unordered_map<roomId, Room> system
//     me hai, hotelId se filter). Ye design choice: rooms ka ek central store,
//     hotel se association reverse-lookup se. Alternative: hotel me vector<Room*>.
//     Central store se room ko id se O(1) dhoondhna aasaan (booking me chahiye).
// ============================================================================
#ifndef OYO_HOTEL_BOOKING_LLD_MODELS_HOTEL_H
#define OYO_HOTEL_BOOKING_LLD_MODELS_HOTEL_H

#include <string>
#include <utility>

namespace oyo_hotel_lld {

using namespace std;

class Hotel {
public:
    Hotel(string hotelId, string name, string city, string address, double rating)
        : hotelId_(std::move(hotelId)),
          name_(std::move(name)),
          city_(std::move(city)),       // search-by-city ke liye
          address_(std::move(address)),
          rating_(rating) {}            // search me sort key

    const string &getHotelId() const { return hotelId_; }
    const string &getName() const { return name_; }
    const string &getCity() const { return city_; }
    const string &getAddress() const { return address_; }
    double getRating() const { return rating_; }

private:
    string hotelId_; // "H1" — unique
    string name_;
    string city_;    // "Jaipur"
    string address_;
    double rating_;  // 4.2 (search me best-first sort)
};

}  // namespace oyo_hotel_lld

#endif
