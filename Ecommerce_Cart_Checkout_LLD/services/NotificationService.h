#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_NOTIFICATIONSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_NOTIFICATIONSERVICE_H

#include <bits/stdc++.h>

#include "../models/Order.h"

namespace ecommerce_checkout_lld {

using namespace std;

class NotificationService {
public:
    void sendOrderConfirmed(const string &email, const Order &order) const {
        cout << "[Email] Order confirmed for " << email << " | OrderId=" << order.getOrderId()
             << " | Payable=Rs " << order.getPayableAmount() << "\n";
    }

    void sendPaymentFailed(const string &email, const string &orderId, const string &reason) const {
        cout << "[Email] Payment failed for " << email << " | OrderId=" << orderId
             << " | Reason=" << reason << "\n";
    }
};

}  // namespace ecommerce_checkout_lld

#endif
