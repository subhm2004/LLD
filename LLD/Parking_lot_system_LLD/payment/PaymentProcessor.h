// ============================================================================
//  payment/PaymentProcessor.h — Payment ka STRATEGY interface (req #9) [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #9: exit pe fee do tareeke se pay ho sakti — automated machine
//  ya parking agent. Ye do "channels" alag-alag kaam karte hain, par dono ko
//  bas fee process karni hai. Isliye STRATEGY pattern (L8):
//
//    PaymentProcessor (interface) -> processPayment(fee, method)
//         ├── AutomatedMachinePaymentProcessor  (khud machine pe)
//         └── AgentPaymentProcessor             (agent ko)
//
//  ParkingLot ko sirf PaymentProcessor* chahiye — machine hai ya agent, farq
//  nahi. Naya channel (jaise "mobile app payment") add karna ho to nayi
//  strategy class, ParkingLot untouched. OCP ✅
//
//  processPayment ko PaymentMethod (card/cash/upi) bhi milta — kis mode se
//  pay kiya. Return bool = payment success/fail.
// ============================================================================
#ifndef PARKING_LOT_SYSTEM_LLD_PAYMENT_PAYMENTPROCESSOR_H
#define PARKING_LOT_SYSTEM_LLD_PAYMENT_PAYMENTPROCESSOR_H

#include "../enums/PaymentMethod.h"

namespace parking_lot_lld {

class PaymentProcessor {
public:
    virtual ~PaymentProcessor() = default;

    // fee = kitna dena hai, method = card/cash/upi. Return: success?
    virtual bool processPayment(double fee, PaymentMethod method) = 0;
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_PAYMENT_PAYMENTPROCESSOR_H
