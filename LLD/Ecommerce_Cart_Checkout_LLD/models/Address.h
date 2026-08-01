// ============================================================================
//  models/Address.h  —  Delivery address (ek user ke kai ho sakte hain)
// ----------------------------------------------------------------------------
//  Address ka apna id hai AUR usme `userId_` bhi hai — yaani har address kisi
//  ek user ka hota hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `userId_` yahan kyun store hota hai? — SECURITY check ke liye         │
//  │                                                                          │
//  │  Checkout me facade `getAddressOrThrow(addressId, userId)` karta hai jo   │
//  │  do cheez check karta:                                                    │
//  │     1. address exist karta hai?                                           │
//  │     2. ⭐ wo address ISI user ka hai? (address.getUserId() == userId)     │
//  │                                                                          │
//  │  Warna koi user doosre ka addressId bhej ke uske ghar order karwa deta!   │
//  │  (Ownership check — "IDOR" (Insecure Direct Object Reference) attack se   │
//  │  bachav.) Ye chhota check real systems me bahut important hota hai.       │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_ADDRESS_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_ADDRESS_H

#include <string>
#include <utility>

namespace ecommerce_checkout_lld {

using namespace std;

class Address {
public:
    Address(string addressId, string userId, string line1, string city, string pincode)
        : addressId_(std::move(addressId)),
          userId_(std::move(userId)),
          line1_(std::move(line1)),
          city_(std::move(city)),
          pincode_(std::move(pincode)) {}

    const string &getAddressId() const { return addressId_; }
    const string &getUserId() const { return userId_; } // ⭐ ownership check ke liye
    const string &getLine1() const { return line1_; }
    const string &getCity() const { return city_; }
    const string &getPincode() const { return pincode_; }

    string toDisplayString() const { return line1_ + ", " + city_ + " - " + pincode_; }

private:
    string addressId_; // "A1" — unique
    string userId_;    // ⭐ ye address kiska hai (security check)
    string line1_;
    string city_;
    string pincode_;
};

}  // namespace ecommerce_checkout_lld

#endif
