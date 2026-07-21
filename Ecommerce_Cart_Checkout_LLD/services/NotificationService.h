// ============================================================================
//  services/NotificationService.h  —  User ko batao kya hua (email simulation)
// ----------------------------------------------------------------------------
//  Abhi bas `cout` pe print hai, par IMPORTANT baat ye hai ki notification
//  bhejna ek ALAG service hai — checkout ke andar `cout` nahi ghusa hua.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Alag class rakhne ka fayda                                            │
//  │  Kal ko asli email/SMS/push chahiye -> sirf yahi file badlegi.            │
//  │  CheckoutService/facade ko pata hi nahi kaise bheja ja raha hai.          │
//  │                                                                          │
//  │  ⭐ AUR ZAROORI: notification CHECKOUT KE BAAD bheja jaata hai (facade    │
//  │     me), checkout ke ANDAR nahi. Kyun? Kyunki email bhejna FAIL ho sakta  │
//  │     hai — aur email fail hone se order CANCEL nahi hona chahiye! Order    │
//  │     ban chuka, paisa kat chuka; email ek side-effect hai, core txn nahi.  │
//  │  📌 Real system me ise queue pe daal dete hain (async, retry ke saath).   │
//  │     Yahan ye NON-CRITICAL side-effect hai — main flow ke bahar rakha hai. │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Do methods = do events (confirmed / failed). Aage chal ke ye Observer
//     pattern me convert ho sakta hai: NotificationService ek LISTENER ban jaye
//     jo OrderService ke events sune — tab facade ko manually call bhi na karna
//     pade. (Logger_LLD me wahi Observer setup dekha ja sakta hai.)
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_NOTIFICATIONSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_NOTIFICATIONSERVICE_H

#include <iostream>
#include <string>

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
