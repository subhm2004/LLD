// ============================================================================
//  decorators/AddOnDecorator.h — DECORATOR (equipment + service add-ons) [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #8/#9: booking pe equipment/service add-ons jodo, cost badhe.
//  Ye decorator ek IRentalCost ko WRAP karta hai (inner_) aur uske cost me
//  apne add-on ka cost (dailyPrice × days) JODTA hai.
//
//  shared_ptr<IRentalCost> use kiya hai — safe ownership (manual delete nahi,
//  chain automatically cleanup). Ek hi decorator class Equipment aur Service
//  dono handle karti (AddOn ke category se label alag aata) — kyunki dono ka
//  math same hai (base + addon × days). Bas description me "Equipment"/"Service".
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │   getCost() = inner_->getCost() + (addOn.dailyPrice × days)             │
//  │   Chain: BaseRental -> +GPS -> +Driver ... har layer apna add karta     │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_DECORATORS_ADDONDECORATOR_H
#define CAR_RENTAL_SYSTEM_LLD_DECORATORS_ADDONDECORATOR_H

#include <bits/stdc++.h>

#include "../models/AddOn.h"
#include "IRentalCost.h"

using namespace std;

namespace car_rental_lld {

class AddOnDecorator : public IRentalCost {
public:
    AddOnDecorator(shared_ptr<IRentalCost> inner, AddOn addOn, int days)
        : inner_(std::move(inner)), addOn_(std::move(addOn)), days_(days) {}

    // Andar wale ka cost + apne add-on ka cost (per-day × days)
    double getCost() const override {
        return inner_->getCost() + addOn_.getDailyPrice() * days_;
    }

    string getDescription() const override {
        return inner_->getDescription() + " + [" + addOn_.categoryLabel() + ": " + addOn_.getName() + "]";
    }

private:
    shared_ptr<IRentalCost> inner_;  // wrapped cost (base ya dusra decorator)
    AddOn addOn_;
    int days_;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_DECORATORS_ADDONDECORATOR_H
