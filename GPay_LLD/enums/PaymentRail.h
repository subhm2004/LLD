// ============================================================================
//  enums/PaymentRail.h  —  Paisa KAHAN SE jayega (funding source)
// ----------------------------------------------------------------------------
//     LINKED_BANK  -> seedha bank account se (asli UPI aisa hi hota hai)
//     GPAY_WALLET  -> app ke andar wale wallet balance se
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ye enum STRATEGY ka selector hai                                      │
//  │                                                                          │
//  │  "Rail" telecom/banking ki term hai — paisa kis PATRI pe chalega.        │
//  │  Har rail ka debit/credit ka tareeka alag hai:                           │
//  │     bank   -> user ka PRIMARY linked account dhundo, usme se kaato       │
//  │     wallet -> userId se wallet nikaalo, usme se kaato                    │
//  │                                                                          │
//  │  Isi liye har rail ki apni Strategy class hai (IPaymentRailStrategy).    │
//  │  Naya rail (CREDIT_LINE / RUPAY_CARD) aaye to: yahan ek enum value +     │
//  │  ek nayi Strategy class + factory me ek case. Baaki system untouched.    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `enum class` (plain `enum` nahi) — type-safe hai. `PaymentRail::LINKED_BANK`
//     galti se int me convert nahi hoga, aur naam dusre enums se takraayenge nahi.
// ============================================================================
#ifndef GPAY_LLD_ENUMS_PAYMENTRAIL_H
#define GPAY_LLD_ENUMS_PAYMENTRAIL_H

namespace gpay_lld {

enum class PaymentRail {
    LINKED_BANK,  // bank account se (asli UPI)
    GPAY_WALLET   // app ke wallet balance se
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_ENUMS_PAYMENTRAIL_H
