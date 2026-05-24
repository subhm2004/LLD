#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_USER_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_USER_H

#include <bits/stdc++.h>

namespace ecommerce_checkout_lld {

using namespace std;

class User {
public:
    User(string userId, string name, string email)
        : userId_(std::move(userId)), name_(std::move(name)), email_(std::move(email)) {}

    const string &getUserId() const { return userId_; }
    const string &getName() const { return name_; }
    const string &getEmail() const { return email_; }

private:
    string userId_;
    string name_;
    string email_;
};

}  // namespace ecommerce_checkout_lld

#endif
