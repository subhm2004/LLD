#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_ADDRESS_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_ADDRESS_H

#include <bits/stdc++.h>

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
    const string &getUserId() const { return userId_; }
    const string &getLine1() const { return line1_; }
    const string &getCity() const { return city_; }
    const string &getPincode() const { return pincode_; }

    string toDisplayString() const { return line1_ + ", " + city_ + " - " + pincode_; }

private:
    string addressId_;
    string userId_;
    string line1_;
    string city_;
    string pincode_;
};

}  // namespace ecommerce_checkout_lld

#endif
