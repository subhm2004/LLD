// ============================================================================
//  DeliveryOrder.h — Concrete Order #1: khana user ke ADDRESS pe deliver
// ----------------------------------------------------------------------------
//  Order base ko extend karta hai + ek extra field: userAddress (kahan
//  deliver karna hai). getType() "Delivery" return karta hai. Factory isi
//  ko banati hai jab orderType == "Delivery" ho.
// ============================================================================
#ifndef DELIVERY_ORDER_H
#define DELIVERY_ORDER_H

#include "Order.h"
using namespace std;

class DeliveryOrder : public Order {
private:
    string userAddress;

public:
    DeliveryOrder() {
        userAddress = "";
    }

    string getType() const override {
        return "Delivery";
    }

    //Getter and Setters
    void setUserAddress(const string& addr) {
        userAddress = addr;
    }

    string getUserAddress() const {
        return userAddress;
    }
};

#endif // DELIVERY_ORDER_H
