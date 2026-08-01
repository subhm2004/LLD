// ============================================================================
//  models/AddOn.h — Equipment + Service add-ons (requirement #8/#9) [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #8 (Equipment): ski rack, child seat, GPS jaise extra saaman.
//  Requirement #9 (Services): driver hire, Wi-Fi, roadside assistance.
//
//  Dono ka structure same hai — naam + daily price. Isliye ek common AddOn
//  model, aur category (EQUIPMENT ya SERVICE) se distinguish. Ye add-ons
//  booking ke saath jud-te hain aur total cost badhate hain (Decorator pattern
//  se — decorators/ folder me).
//
//  Example: Booking = base rental + [GPS equipment] + [Driver service] ->
//  har add-on cost jodta hai. Real car rental me exactly aisa hi hota
//  ("Add extras to your reservation").
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_MODELS_ADDON_H
#define CAR_RENTAL_SYSTEM_LLD_MODELS_ADDON_H

#include <bits/stdc++.h>

using namespace std;

namespace car_rental_lld {

enum class AddOnCategory { EQUIPMENT, SERVICE };

class AddOn {
public:
    AddOn(string name, double dailyPrice, AddOnCategory category)
        : name_(std::move(name)), dailyPrice_(dailyPrice), category_(category) {}

    const string &getName() const { return name_; }
    double getDailyPrice() const { return dailyPrice_; }
    AddOnCategory getCategory() const { return category_; }

    string categoryLabel() const {
        return category_ == AddOnCategory::EQUIPMENT ? "Equipment" : "Service";
    }

private:
    string name_;
    double dailyPrice_;
    AddOnCategory category_;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_MODELS_ADDON_H
