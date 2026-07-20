// ============================================================================
//  enums/TransactionType.h  —  ATM me kis tarah ki transaction hui
// ----------------------------------------------------------------------------
//  ATM me user do kaam kar sakta hai, to do type:
//
//     BALANCE_INQUIRY -> sirf balance dekha (paisa nahi nikala)
//     CASH_WITHDRAWAL -> cash nikala (paisa account se gaya)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Har action ka ek Transaction record banta hai (audit ke liye)         │
//  │                                                                          │
//  │  Chahe balance check ho ya withdrawal — dono ka ek Transaction log banta  │
//  │  hai (type + amount + status). Isse "kab kya hua" ka poora hisaab rehta.  │
//  │  Balance inquiry ka amount 0 hota hai (paisa nahi hilta).                 │
//  │                                                                          │
//  │  💡 Naya feature (DEPOSIT / TRANSFER / MINI_STATEMENT) add karna ho to    │
//  │     yahan ek entry add karo — enum extensible hai.                       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  `enum class` (scoped) hai — `TransactionType::CASH_WITHDRAWAL` likhna padta,
//  naam bahar leak nahi hota aur galti se int me convert nahi hota (type safety).
// ============================================================================
#ifndef ATM_LLD_ENUMS_TRANSACTIONTYPE_H
#define ATM_LLD_ENUMS_TRANSACTIONTYPE_H

namespace atm_lld {

enum class TransactionType {
    BALANCE_INQUIRY, // balance dekha (amount 0)
    CASH_WITHDRAWAL  // cash nikala (account se debit)
};

} // namespace atm_lld

#endif // ATM_LLD_ENUMS_TRANSACTIONTYPE_H
