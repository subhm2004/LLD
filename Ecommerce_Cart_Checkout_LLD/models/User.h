// ============================================================================
//  models/User.h  —  Ek registered user (jo order karta hai)
// ----------------------------------------------------------------------------
//  Simple data holder: id, naam, email. Email pe order-confirmation notification
//  jaata hai (NotificationService).
//
//  ⭐ IMMUTABLE (sirf getters) — value object hai, copy/share safe.
//  📌 Address alag model hai (Address.h) kyunki ek user ke KAI addresses ho sakte
//     hain (ghar, office). Isi liye user me address embed nahi kiya.
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_USER_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_USER_H

#include <string>
#include <utility>

namespace ecommerce_checkout_lld {

using namespace std;

class User {
public:
    User(string userId, string name, string email)
        : userId_(std::move(userId)), name_(std::move(name)), email_(std::move(email)) {}

    const string &getUserId() const { return userId_; }
    const string &getName() const { return name_; }
    const string &getEmail() const { return email_; } // notifications yahan jaate hain

private:
    string userId_; // "U1" — unique
    string name_;
    string email_;
};

}  // namespace ecommerce_checkout_lld

#endif
