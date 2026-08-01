// ============================================================================
//  payment/AgentPaymentProcessor.h — Concrete Strategy #2 [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #9 ka doosra option — parking AGENT (banda) ko payment.
//  Customer exit booth pe baithe agent ko fee deta (card/cash/upi). Agent
//  manually process karta aur receipt deta. Human-assisted (machine nahi).
//
//  Same PaymentProcessor (Strategy) interface — ParkingLot ke liye machine
//  aur agent interchangeable hain (dono processPayment dete hain). Sirf
//  andar ka "kaise" alag (self-service vs human).
// ============================================================================
#ifndef PARKING_LOT_SYSTEM_LLD_PAYMENT_AGENTPAYMENTPROCESSOR_H
#define PARKING_LOT_SYSTEM_LLD_PAYMENT_AGENTPAYMENTPROCESSOR_H

#include <bits/stdc++.h>

#include "PaymentProcessor.h"

using namespace std;

namespace parking_lot_lld {

class AgentPaymentProcessor : public PaymentProcessor {
public:
    bool processPayment(double fee, PaymentMethod method) override {
        cout << "[Agent] Collecting payment of Rs " << fee
             << " via " << paymentMethodToString(method) << " (parking agent).\n";
        cout << "[Agent] Payment received. Handing over receipt.\n";
        return true;
    }
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_PAYMENT_AGENTPAYMENTPROCESSOR_H
