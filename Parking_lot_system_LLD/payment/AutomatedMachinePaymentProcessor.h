// ============================================================================
//  payment/AutomatedMachinePaymentProcessor.h — Concrete Strategy #1 [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #9 ka pehla option — AUTOMATED MACHINE pe payment.
//  Customer khud machine pe ticket daal ke card/cash/upi se pay karta.
//  Machine automated hai — koi banda nahi, self-service.
//
//  Ye PaymentProcessor (Strategy) ko implement karta hai. Real me yahan
//  card reader / cash acceptor / UPI QR ka integration hota; demo me print.
// ============================================================================
#ifndef PARKING_LOT_SYSTEM_LLD_PAYMENT_AUTOMATEDMACHINEPAYMENTPROCESSOR_H
#define PARKING_LOT_SYSTEM_LLD_PAYMENT_AUTOMATEDMACHINEPAYMENTPROCESSOR_H

#include <bits/stdc++.h>

#include "PaymentProcessor.h"

using namespace std;

namespace parking_lot_lld {

class AutomatedMachinePaymentProcessor : public PaymentProcessor {
public:
    bool processPayment(double fee, PaymentMethod method) override {
        cout << "[AutomatedMachine] Processing payment of Rs " << fee
             << " via " << paymentMethodToString(method) << " (self-service machine).\n";
        cout << "[AutomatedMachine] Payment successful. Please collect your receipt.\n";
        return true;
    }
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_PAYMENT_AUTOMATEDMACHINEPAYMENTPROCESSOR_H
