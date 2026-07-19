// ============================================================================
//  services/PricingService.h — Pricing ka CONTEXT (Strategy pattern ka user)
// ----------------------------------------------------------------------------
//  Pehle ye seedha days×rate karta tha. Ab ye STRATEGY CONTEXT hai — actual
//  base pricing PricingStrategy ko delegate karta (default DailyPricing).
//  Naya pricing model plug karo (WeekendSurge) to sirf strategy inject,
//  PricingService untouched.
//
//  Do kaam:
//    calculateRentAmount() -> base amount (days validate + strategy delegate)
//    calculateFine()       -> overdue fine (requirement #10: late return pe fine)
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_SERVICES_PRICINGSERVICE_H
#define CAR_RENTAL_SYSTEM_LLD_SERVICES_PRICINGSERVICE_H

#include <bits/stdc++.h>

#include "../strategies/DailyPricingStrategy.h"
#include "../strategies/PricingStrategy.h"

using namespace std;

namespace car_rental_lld {

class PricingService {
public:
    // Default strategy: simple daily. Constructor se dusri inject kar sakte.
    PricingService() : strategy_(make_shared<DailyPricingStrategy>()) {}
    explicit PricingService(shared_ptr<PricingStrategy> strategy) : strategy_(std::move(strategy)) {}

    // Base rent (requirement #4). Days = inclusive range.
    double calculateRentAmount(double dailyRate, int startDay, int endDay) const {
        if (startDay > endDay) {
            throw runtime_error("Invalid booking dates");
        }
        const int days = endDay - startDay + 1;
        return strategy_->calculateBaseAmount(dailyRate, days);  // STRATEGY delegate
    }

    // REQUIREMENT #10 — overdue fine: due date ke baad har extra din par fine.
    // Fine = extra days × (dailyRate ka 1.5x — penalty). Late return mehnga!
    double calculateFine(double dailyRate, int dueDay, int actualReturnDay) const {
        if (actualReturnDay <= dueDay) {
            return 0.0;  // time pe ya jaldi return — koi fine nahi
        }
        const int overdueDays = actualReturnDay - dueDay;
        return overdueDays * dailyRate * 1.5;  // 1.5x penalty rate
    }

private:
    shared_ptr<PricingStrategy> strategy_;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_SERVICES_PRICINGSERVICE_H
