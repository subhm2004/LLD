// ============================================================================
//  ScheduledOrderFactory.h — Concrete Factory #2: FUTURE time wala order
// ----------------------------------------------------------------------------
//  NowOrderFactory jaisa hi, PAR scheduled time constructor se aata hai
//  (user-given future time, current nahi). checkoutScheduled() isse use karta.
//  Baaki sab same: orderType se Delivery/Pickup, phir order fields set.
// ============================================================================
#ifndef SCHEDULED_ORDER_FACTORY_H
#define SCHEDULED_ORDER_FACTORY_H

#include "OrderFactory.h"
#include "../models/DeliveryOrder.h"
#include "../models/PickupOrder.h"
#include "../utils/TimeUtils.h"
using namespace std;

class ScheduledOrderFactory : public OrderFactory {
private:
    string scheduleTime;
public:
    ScheduledOrderFactory(string scheduleTime) {
        this->scheduleTime = scheduleTime;
    }

    Order* createOrder(User* user, Cart* cart, Restaurant* restaurant, const vector<MenuItem>& menuItems,
                        PaymentStrategy* paymentStrategy, double totalCost, const string& orderType) override {
        Order* order = nullptr;

        if(orderType == "Delivery") {
            auto deliveryOrder = new DeliveryOrder();
            deliveryOrder->setUserAddress(user->getAddress());
            order = deliveryOrder;
        }
        else {
            auto pickupOrder = new PickupOrder();
            pickupOrder->setRestaurantAddress(restaurant->getLocation());
            // BUG FIX: pehle yahan `order = pickupOrder;` MISSING tha —
            // isse pickup order me `order` nullptr reh jaata aur neeche
            // order->setUser() pe CRASH ho jaata (segfault). NowOrderFactory
            // me ye line sahi thi, yahan copy karte waqt chhoot gayi thi.
            order = pickupOrder;
        }
        order->setUser(user);
        order->setRestaurant(restaurant);
        order->setItems(menuItems);
        order->setPaymentStrategy(paymentStrategy);
        order->setScheduled(scheduleTime);
        order->setTotal(totalCost);
        return order;
    }
};

#endif // SCHEDULED_ORDER_FACTORY_H
