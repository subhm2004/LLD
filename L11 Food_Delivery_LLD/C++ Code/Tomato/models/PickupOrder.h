// PickupOrder.h — Order ka woh type jahan user khud restaurant se PICKUP karta
// hai (koi delivery address nahi). Order base ko extend karta hai.
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
