#ifndef OYO_HOTEL_BOOKING_LLD_MODELS_GUEST_H
#define OYO_HOTEL_BOOKING_LLD_MODELS_GUEST_H

#include <bits/stdc++.h>

namespace oyo_hotel_lld {

using namespace std;

class Guest {
public:
    Guest(string guestId, string name, string phone, string email)
        : guestId_(std::move(guestId)),
          name_(std::move(name)),
          phone_(std::move(phone)),
          email_(std::move(email)) {}

    const string &getGuestId() const { return guestId_; }
    const string &getName() const { return name_; }
    const string &getPhone() const { return phone_; }
    const string &getEmail() const { return email_; }

private:
    string guestId_;
    string name_;
    string phone_;
    string email_;
};

}  // namespace oyo_hotel_lld

#endif
