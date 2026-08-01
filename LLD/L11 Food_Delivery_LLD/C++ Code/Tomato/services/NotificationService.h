// ============================================================================
//  NotificationService.h — Order confirm hone par receipt/notification bhejo
// ----------------------------------------------------------------------------
//  Payment success ke baad order ki poori summary print karta hai (id,
//  customer, restaurant, items, total, time). SEPARATION OF CONCERNS:
//  notification ka kaam alag class me — Order/Payment isse mixed nahi.
//  Kal SMS/email/push add karna ho to bas yahi class badlegi (ya Strategy/
//  Observer laga sakte ho — L12/L14 wala). Abhi static print-only.
// ============================================================================
#ifndef NOTIFICATION_SERVICE_H
#define NOTIFICATION_SERVICE_H

#include <iostream>
#include "../models/Order.h"
using namespace std;

class NotificationService {
public:
    static void notify(Order* order) {
        cout << "\nNotification: New " << order->getType() << " order placed!" << endl;
        cout << "---------------------------------------------" << endl;
        cout << "Order ID: " << order->getOrderId() << endl;
        cout << "Customer: " << order->getUser()->getName() << endl;
        cout << "Restaurant: " << order->getRestaurant()->getName() << endl;
        cout << "Items Ordered:\n";

        const vector<MenuItem>& items = order->getItems();
        for (const auto& item : items) {
            cout << "   - " << item.getName() << " (₹" << item.getPrice() << ")\n";
        }

        cout << "Total: ₹" << order->getTotal() << endl;
        cout << "Scheduled For: " << order->getScheduled() << endl;
        cout << "Payment: Done" << endl;
        cout << "---------------------------------------------" << endl;
    }
};

#endif // NOTIFICATION_SERVICE_H
