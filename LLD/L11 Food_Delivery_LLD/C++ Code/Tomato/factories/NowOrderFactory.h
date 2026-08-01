// ============================================================================
//  NowOrderFactory.h — Concrete Factory #1: ABHI wala order (current time)
// ----------------------------------------------------------------------------
//  OrderFactory ka implementation — order banata hai AUR uska scheduled time
//  TimeUtils::getCurrentTime() (ABHI) set karta hai.
//  Andar orderType dekh ke Delivery ya Pickup order banata hai (delivery me
//  user address, pickup me restaurant location set hota hai). Yahi ek jagah
//  hai jahan concrete order classes ka naam aata hai — client insse door.
// ============================================================================
#ifndef NOW_ORDER_FACTORY_H
#define NOW_ORDER_FACTORY_H

#include "OrderFactory.h"
#include "../models/DeliveryOrder.h"
#include "../models/PickupOrder.h"
#include "../utils/TimeUtils.h"
using namespace std;

class NowOrderFactory : public OrderFactory {
public:
    Order* createOrder(User* user, Cart* cart, Restaurant* restaurant, const vector<MenuItem>& menuItems,
                       PaymentStrategy* paymentStrategy, double totalCost, const string& orderType) override {
        Order* order = nullptr;
        if (orderType == "Delivery") {
            auto deliveryOrder = new DeliveryOrder();
            deliveryOrder->setUserAddress(user->getAddress());
            order = deliveryOrder;
        }
        else {
            auto pickupOrder = new PickupOrder();
            pickupOrder->setRestaurantAddress(restaurant->getLocation());
            order = pickupOrder;
        }
        order->setUser(user);
        order->setRestaurant(restaurant);
        order->setItems(menuItems);
        order->setPaymentStrategy(paymentStrategy);
        order->setScheduled(TimeUtils::getCurrentTime());
        order->setTotal(totalCost);
        return order;
    }
};

#endif // NOW_ORDER_FACTORY_H
