#ifndef OYO_HOTEL_BOOKING_LLD_SERVICES_PRICINGSERVICE_H
#define OYO_HOTEL_BOOKING_LLD_SERVICES_PRICINGSERVICE_H

#include <bits/stdc++.h>

#include "../strategies/IPricingStrategy.h"
#include "../strategies/StandardPricingStrategy.h"

namespace oyo_hotel_lld {

using namespace std;

class PricingService {
public:
    PricingService() : strategy_(make_unique<StandardPricingStrategy>()) {}

    void setStrategy(unique_ptr<IPricingStrategy> strategy) {
        if (!strategy) {
            throw invalid_argument("Pricing strategy cannot be null");
        }
        strategy_ = std::move(strategy);
    }

    double calculateStayAmount(double pricePerNight, int checkInDay, int checkOutDay) const {
        return strategy_->calculateTotal(pricePerNight, checkInDay, checkOutDay);
    }

private:
    unique_ptr<IPricingStrategy> strategy_;
};

}  // namespace oyo_hotel_lld

#endif
