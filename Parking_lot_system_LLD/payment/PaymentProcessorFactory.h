// ============================================================================
//  payment/PaymentProcessorFactory.h — FACTORY (L9) [NAYA]
// ----------------------------------------------------------------------------
//  PaymentChannel enum (AUTOMATED_MACHINE / AGENT) do, sahi PaymentProcessor
//  (Strategy) object bana ke lo. Client ko concrete class ka naam nahi pata —
//  bas channel bolta hai, factory sahi processor banati hai.
//
//  ⭐ FACTORY + STRATEGY combo (common pattern pairing): Factory creation
//  handle karti (kaunsa banao), Strategy behavior (kaise pay karo). ParkingLot
//  factory se processor maangta, phir usse pay karwata. Naya channel add =
//  ek case + nayi strategy class.
//
//  Ownership: factory `new` karti hai, caller delete ka zimmedar (ya modern
//  C++ me unique_ptr return karna better — demo simplicity ke liye raw).
// ============================================================================
#ifndef PARKING_LOT_SYSTEM_LLD_PAYMENT_PAYMENTPROCESSORFACTORY_H
#define PARKING_LOT_SYSTEM_LLD_PAYMENT_PAYMENTPROCESSORFACTORY_H

#include "../enums/PaymentChannel.h"
#include "AgentPaymentProcessor.h"
#include "AutomatedMachinePaymentProcessor.h"
#include "PaymentProcessor.h"

namespace parking_lot_lld {

class PaymentProcessorFactory {
public:
    static PaymentProcessor *create(PaymentChannel channel) {
        if (channel == PaymentChannel::AGENT) {
            return new AgentPaymentProcessor();
        }
        // default: automated machine
        return new AutomatedMachinePaymentProcessor();
    }
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_PAYMENT_PAYMENTPROCESSORFACTORY_H
