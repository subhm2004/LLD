// ============================================================================
//  enums/PaymentMethod.h — Payment ke tareeke (requirement #11) [NAYA]
// ----------------------------------------------------------------------------
//  Exit pe fee kaise pay karein — requirements ke hisaab se:
//    CREDIT_CARD -> credit card se
//    DEBIT_CARD  -> debit card se
//    CASH        -> nakad
//    UPI         -> UPI apps (GPay/PhonePe jaise)
//
//  DHYAN DO — PaymentMethod (kaise pay karo) aur payment CHANNEL (kahan pay
//  karo: automated machine ya agent) DO alag cheezein hain:
//    PaymentMethod  = card/cash/upi  (enum, ye file)
//    Payment channel = machine/agent (Strategy — payment/ folder me)
//  Jaise: "automated machine pe CREDIT_CARD se" ya "agent ko CASH se".
// ============================================================================
#ifndef PARKING_LOT_SYSTEM_LLD_ENUMS_PAYMENTMETHOD_H
#define PARKING_LOT_SYSTEM_LLD_ENUMS_PAYMENTMETHOD_H

#include <bits/stdc++.h>

using namespace std;

namespace parking_lot_lld {

enum class PaymentMethod { CREDIT_CARD, DEBIT_CARD, CASH, UPI };

inline string paymentMethodToString(PaymentMethod method) {
    switch (method) {
        case PaymentMethod::CREDIT_CARD: return "CREDIT_CARD";
        case PaymentMethod::DEBIT_CARD: return "DEBIT_CARD";
        case PaymentMethod::CASH: return "CASH";
        case PaymentMethod::UPI: return "UPI";
    }
    return "UNKNOWN";
}

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_ENUMS_PAYMENTMETHOD_H
