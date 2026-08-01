#ifndef UBER_LLD_MODELS_RIDER_H
#define UBER_LLD_MODELS_RIDER_H

#include <bits/stdc++.h>

using namespace std;

namespace uber_lld {

class Rider {
public:
    Rider(string riderId, string name, string phone)
        : riderId_(std::move(riderId)), name_(std::move(name)), phone_(std::move(phone)) {}

    const string &getId() const { return riderId_; }
    const string &getName() const { return name_; }
    const string &getPhone() const { return phone_; }

private:
    string riderId_;
    string name_;
    string phone_;
};

} // namespace uber_lld

#endif // UBER_LLD_MODELS_RIDER_H
