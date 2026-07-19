// ============================================================================
//  Order.h — ABSTRACT base order (DeliveryOrder / PickupOrder ka parent)
// ----------------------------------------------------------------------------
//  Ek order ka pura data: user, restaurant, items, total, scheduled time,
//  aur PAYMENT STRATEGY (composition — Order khud pay nahi karta, strategy
//  se karwata hai!). getType() pure virtual hai — subclass "Delivery" ya
//  "Pickup" batati hai (polymorphism).
//
//  ⭐ processPayment() me STRATEGY delegation dekho:
//    paymentStrategy->pay(total)  — Order ko pata nahi UPI hai ya Card,
//    bas strategy ko bol deta hai. Yahi Strategy + composition ka core.
//
//  nextOrderId static counter se har order ko unique id milti hai (1,2,3...).
//  Virtual destructor hai — base Order* se derived delete safe.
// ============================================================================
#ifndef ORDER_H
#define ORDER_H

#include <iostream>
#include <string>
#include <vector>
#include "User.h"
#include "Restaurant.h"
#include "MenuItem.h"
#include "../strategies/PaymentStrategy.h"
#include "../utils/TimeUtils.h"
using namespace std;

class Order {
protected:
    static int nextOrderId;
    int orderId;
    User* user;
    Restaurant* restaurant;
    vector<MenuItem> items;
    PaymentStrategy* paymentStrategy;
    double total;
    string scheduled;

public:
    Order() {
        user = nullptr;
        restaurant = nullptr;
        paymentStrategy = nullptr;
        total = 0.0;
        scheduled = "";
        orderId = ++nextOrderId;
    }

    virtual ~Order() {
        delete paymentStrategy;
    }

    bool processPayment() {
        if (paymentStrategy) {
            paymentStrategy->pay(total);
            return true;
        } else {
            cout << "Please choose a payment mode first" << endl;
            return false;
        }
    }

    virtual string getType() const = 0;

    //Getter and Setters
    int getOrderId() const {
        return orderId;
    }

    void setUser(User* u) {
        user = u;
    }

    User* getUser() const {
        return user;
    }

    void setRestaurant(Restaurant* r) {
        restaurant = r;
    }

    Restaurant* getRestaurant() const {
        return restaurant;
    }

    void setItems(const vector<MenuItem>& its) {
        items = its;
        total = 0;
        for (auto &i : items) {
            total += i.getPrice();
        }
    }

    const vector<MenuItem>& getItems() const {
        return items;
    }

    void setPaymentStrategy(PaymentStrategy* p) {
        paymentStrategy = p;
    }

    void setScheduled(const string& s) {
        scheduled = s;
    }

    string getScheduled() const {
        return scheduled;
    }

    double getTotal() const {
        return total;
    }

    void setTotal(int total) {
        this->total = total;
    }
};

int Order::nextOrderId = 0;

#endif // ORDER_H
