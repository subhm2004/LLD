#ifndef CAR_RENTAL_SYSTEM_LLD_MODELS_CUSTOMER_H
#define CAR_RENTAL_SYSTEM_LLD_MODELS_CUSTOMER_H

#include <bits/stdc++.h>

using namespace std;

namespace car_rental_lld {

class Customer {
public:
    Customer(string customerId, string name, string licenseNumber)
        : customerId_(std::move(customerId)),
          name_(std::move(name)),
          licenseNumber_(std::move(licenseNumber)) {}

    const string &getId() const { return customerId_; }
    const string &getName() const { return name_; }
    const string &getLicenseNumber() const { return licenseNumber_; }

private:
    string customerId_;
    string name_;
    string licenseNumber_;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_MODELS_CUSTOMER_H
