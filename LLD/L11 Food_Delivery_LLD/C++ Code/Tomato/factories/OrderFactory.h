// ============================================================================
//  OrderFactory.h — FACTORY METHOD interface: order banane ka contract (L9!)
// ----------------------------------------------------------------------------
//  createOrder() ka abstract contract — kaunsa order banega ye SUBCLASS
//  decide karti hai (NowOrderFactory = abhi wala, ScheduledOrderFactory =
//  future time wala). TomatoApp sirf OrderFactory* jaanta hai — concrete
//  factory inject hoti hai (checkoutNow vs checkoutScheduled me).
//
//  DHYAN DO — YAHAN DO-LEVEL VARIATION hai (isliye Factory Method perfect):
//    1. WHEN: abhi ya scheduled  -> kaunsi FACTORY (Now/Scheduled)
//    2. HOW:  delivery ya pickup -> factory ke andar orderType se decide
//  Ek hi createOrder() interface, char alag combinations bina if-else
//  jungle ke. Naya order-timing (jaise "recurring") = nayi factory class!
// ============================================================================
#ifndef ORDER_FACTORY_H
#define ORDER_FACTORY_H

#include "../models/Order.h"
#include "../models/Cart.h"
#include "../models/Restaurant.h"
#include "../strategies/PaymentStrategy.h"
#include <vector>
#include <string>
using namespace std;

class OrderFactory {
public:
    virtual Order* createOrder(User* user, Cart* cart, Restaurant* restaurant, const vector<MenuItem>& menuItems,
                                PaymentStrategy* paymentStrategy, double totalCost, const string& orderType) = 0;
    virtual ~OrderFactory() {}
};

#endif // ORDER_FACTORY_H
