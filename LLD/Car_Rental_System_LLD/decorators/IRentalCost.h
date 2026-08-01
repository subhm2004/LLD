// ============================================================================
//  decorators/IRentalCost.h — DECORATOR (L13) ka Component interface [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #8/#9: booking me equipment + services add karo, jo cost
//  BADHATE hain. Ye "base ke upar extras layer karna" — bilkul DECORATOR
//  pattern ka classic use case!
//
//  IRentalCost = component interface. getCost() aur getDescription() dega.
//  BaseRental = base cost (vehicle × days). Uske upar EquipmentDecorator /
//  ServiceDecorator wrap karke har add-on cost jodta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  COST BUILD-UP (decorator layering):                                    │
//  │   BaseRental (car 3 days = 7500)                                        │
//  │     wrapped in EquipmentDecorator (GPS +300)  = 7800                    │
//  │       wrapped in ServiceDecorator (Driver +1500) = 9300                 │
//  │   getCost() andar se bahar chalta: base -> +equip -> +service           │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  Fayda: koi bhi combination possible (sirf GPS, sirf driver, dono, kuch
//  nahi) bina har combo ke liye nayi class banaye. Yahi Decorator ka point.
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_DECORATORS_IRENTALCOST_H
#define CAR_RENTAL_SYSTEM_LLD_DECORATORS_IRENTALCOST_H

#include <bits/stdc++.h>

using namespace std;

namespace car_rental_lld {

class IRentalCost {
public:
    virtual ~IRentalCost() = default;
    virtual double getCost() const = 0;
    virtual string getDescription() const = 0;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_DECORATORS_IRENTALCOST_H
