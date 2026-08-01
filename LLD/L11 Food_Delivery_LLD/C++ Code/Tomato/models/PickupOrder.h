// ============================================================================
//  PickupOrder.h — Concrete Order #2: user khud restaurant se PICKUP kare
// ----------------------------------------------------------------------------
//  Order base ko extend karta hai + extra field: restaurantAddress (kahan
//  se pickup karna hai). getType() "Pickup" return karta hai. DeliveryOrder
//  ka mirror — sirf address ka meaning alag (delivery addr vs pickup point).
// ============================================================================
#ifndef PICKUP_ORDER_H
#define PICKUP_ORDER_H

#include "Order.h"
using namespace std;

class PickupOrder : public Order {
private:
    string restaurantAddress;

public:
    PickupOrder() {
        restaurantAddress = "";
    }

    string getType() const override {
        return "Pickup";
    }

    //Getter and Setters
    void setRestaurantAddress(const string& addr) {
        restaurantAddress = addr;
    }

    string getRestaurantAddress() const {
        return restaurantAddress;
    }
};

#endif // PICKUP_ORDER_H
