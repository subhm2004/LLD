#ifndef CAR_RENTAL_SYSTEM_LLD_SERVICES_PRICINGSERVICE_H
#define CAR_RENTAL_SYSTEM_LLD_SERVICES_PRICINGSERVICE_H

#include <bits/stdc++.h>

using namespace std;

namespace car_rental_lld {

class PricingService {
public:
    double calculateRentAmount(double dailyRate, int startDay, int endDay) const {
        if (startDay > endDay) {
            throw runtime_error("Invalid booking dates");
        }
        const int days = endDay - startDay + 1;
        return days * dailyRate;
    }
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_SERVICES_PRICINGSERVICE_H
