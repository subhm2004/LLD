// ============================================================================
//  enums/TransactionStatus.h  —  Ek transaction abhi kis haal me hai
// ----------------------------------------------------------------------------
//  Ek transaction ka chhota safar (mini state machine):
//
//     INITIATED -> transaction shuru hui, abhi result nahi aaya
//     SUCCESS   -> bina error ke poori ho gayi
//     FAILED    -> kisi validation/error ki wajah se fail ho gayi
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FAILED transactions bhi LOG hoti hain — ye zaroori hai               │
//  │                                                                          │
//  │  Insufficient balance ya notes na hone par bhi ek Transaction banti hai,  │
//  │  bas uska status FAILED hota hai. Kyun? Audit trail ke liye — "user ne    │
//  │  koshish ki thi par fail hui" ka record rehna chahiye (real bank isi se   │
//  │  fraud/pattern detect karta hai). Isi liye ATMSystem har fail case par    │
//  │  createTransaction(..., success=false) bhi call karta hai.               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Har naya Transaction constructor me INITIATED se shuru hota hai, aur baad
//     me markSuccess()/markFailed() se apni final state me jaata hai.
// ============================================================================
#ifndef ATM_LLD_ENUMS_TRANSACTIONSTATUS_H
#define ATM_LLD_ENUMS_TRANSACTIONSTATUS_H

namespace atm_lld {

enum class TransactionStatus {
    INITIATED, // shuru hui, process ho rahi hai
    SUCCESS,   // bina error ke complete
    FAILED     // validation/error ki wajah se fail
};

} // namespace atm_lld

#endif // ATM_LLD_ENUMS_TRANSACTIONSTATUS_H
