// ============================================================================
//  enums/TransactionStatus.h  —  Transaction ki haalat
// ----------------------------------------------------------------------------
//     INITIATED -> transaction ban gaya, par paisa abhi hila nahi
//     SUCCESS   -> debit + credit dono ho gaye ✅
//     FAILED    -> beech me kuch toota (paisa wapas kar diya gaya)
//     PENDING   -> MoneyRequest ke liye: request bheji hai, jawab ka intezaar
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ INITIATED alag state kyun? (seedha SUCCESS/FAILED kyun nahi)          │
//  │                                                                          │
//  │  TransactionFactory transaction INITIATED me banata hai — paisa hilne se │
//  │  PEHLE. Iska fayda: server abhi crash ho jaye to bhi ledger me ek record  │
//  │  bacha hai jo kehta hai "ye transaction shuru hui thi". Reconciliation    │
//  │  job baad me dekh sakti hai ki iska kya hua.                             │
//  │  Bina INITIATED ke, crash hone pe transaction ka NAAM-O-NISHAAN hi na     │
//  │  hota — aur agar paisa kat chuka hota to wo silently gayab. 😱            │
//  │  📌 Paise wale systems me "intent pehle likho, phir kaam karo" ek         │
//  │     standard rule hai (write-ahead / outbox soch).                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Ye enum DO cheezon ke liye reuse hota hai — Transaction aur MoneyRequest.
//     MoneyRequest me: PENDING (intezaar) -> SUCCESS (bhar diya) / FAILED (cancel).
//     ⚠ Reuse ka trade-off: Transaction kabhi PENDING nahi hota, MoneyRequest
//        kabhi INITIATED nahi hota. Yaani dono ke paas aisi values hain jo unke
//        liye matlab nahi rakhti. Chhote system me theek, bada hone pe alag
//        `RequestStatus` enum banana behtar hai.
// ============================================================================
#ifndef GPAY_LLD_ENUMS_TRANSACTIONSTATUS_H
#define GPAY_LLD_ENUMS_TRANSACTIONSTATUS_H

namespace gpay_lld {

enum class TransactionStatus {
    INITIATED,  // ⭐ ban gaya, paisa abhi hila nahi (crash-safe marker)
    SUCCESS,    // debit + credit dono complete
    FAILED,     // toota (rollback ho chuka)
    PENDING     // MoneyRequest: jawab ka intezaar
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_ENUMS_TRANSACTIONSTATUS_H
