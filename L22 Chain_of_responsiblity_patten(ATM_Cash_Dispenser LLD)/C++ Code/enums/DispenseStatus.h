// ============================================================================
//  enums/DispenseStatus.h — Withdrawal ka RESULT status
// ----------------------------------------------------------------------------
//  Service withdraw() ka return type. Client ko batata hai kya hua:
//    SUCCESS               -> dispense ho gaya
//    INVALID_AMOUNT        -> galat amount (0, negative, ya ₹100 ka multiple nahi)
//    INSUFFICIENT_ATM_CASH -> chain init nahi thi / notes kam the
//  Result ko status-code se return karna (bool ke bajaye) — clean API
//  design, caller sahi se error handle kar sakta hai. Monolith me sirf
//  cout tha, koi return status nahi — ye layered version behtar hai.
// ============================================================================
#ifndef COR_ATM_ENUMS_DISPENSE_STATUS_H
#define COR_ATM_ENUMS_DISPENSE_STATUS_H

namespace cor_atm {

enum class DispenseStatus {
    SUCCESS,
    INVALID_AMOUNT,
    INSUFFICIENT_ATM_CASH
};

} // namespace cor_atm

#endif // COR_ATM_ENUMS_DISPENSE_STATUS_H
