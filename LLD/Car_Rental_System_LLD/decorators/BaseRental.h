// ============================================================================
//  decorators/BaseRental.h — Decorator ka CONCRETE COMPONENT (base cost) [NAYA]
// ----------------------------------------------------------------------------
//  Ye decorator chain ka SABSE ANDAR wala (base) hai — vehicle ka basic
//  rental cost (bina add-ons ke). Base amount PRICING STRATEGY se pehle hi
//  calculate ho ke aata hai (Strategy + Decorator ka clean separation:
//  Strategy = base kaise price ho, Decorator = extras kaise judein).
//  Ye khud kisi ko wrap nahi karta (base hai); iske UPAR AddOnDecorator wrap hote.
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_DECORATORS_BASERENTAL_H
#define CAR_RENTAL_SYSTEM_LLD_DECORATORS_BASERENTAL_H

#include <bits/stdc++.h>

#include "IRentalCost.h"

using namespace std;

namespace car_rental_lld {

class BaseRental : public IRentalCost {
public:
    // baseAmount pehle se calculated (PricingStrategy se); days sirf description ke liye
    BaseRental(double baseAmount, int days) : baseAmount_(baseAmount), days_(days) {}

    double getCost() const override { return baseAmount_; }

    string getDescription() const override {
        return "Base rental (" + to_string(days_) + " days)";
    }

private:
    double baseAmount_;
    int days_;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_DECORATORS_BASERENTAL_H
