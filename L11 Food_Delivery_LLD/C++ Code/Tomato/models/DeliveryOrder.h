// DeliveryOrder.h — Order ka woh type jahan khana user ke address pe DELIVER
// hota hai (delivery address extra field). Order base ko extend karta hai.
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
