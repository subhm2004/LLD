// ============================================================================
//  models/Guest.h  —  Ek guest (jo booking karta hai)
// ----------------------------------------------------------------------------
//  Simple data holder: guest ki id, naam, phone, email. Notifications inhi
//  contact details pe jaate hain (email/SMS — NotificationService).
//
//  ⭐ IMMUTABLE — sirf getters, koi setter. Ek baar guest register ho gaya, uski
//     details is model me badalti nahi (profile update alag concern hota). Value
//     objects ko immutable rakhna safe (copy/share sab bina dar ke).
// ============================================================================
#ifndef OYO_HOTEL_BOOKING_LLD_MODELS_GUEST_H
#define OYO_HOTEL_BOOKING_LLD_MODELS_GUEST_H

#include <string>
#include <utility>

namespace oyo_hotel_lld {

using namespace std;

class Guest {
public:
    Guest(string guestId, string name, string phone, string email)
        : guestId_(std::move(guestId)),
          name_(std::move(name)),
          phone_(std::move(phone)),   // SMS notifications ke liye
          email_(std::move(email)) {} // email notifications ke liye

    const string &getGuestId() const { return guestId_; }
    const string &getName() const { return name_; }
    const string &getPhone() const { return phone_; }
    const string &getEmail() const { return email_; }

private:
    string guestId_; // "G1" — unique
    string name_;
    string phone_;
    string email_;
};

}  // namespace oyo_hotel_lld

#endif
