// ============================================================================
//  enums/TransactionType.h  —  Transaction kis FLOW se aayi
// ----------------------------------------------------------------------------
//     P2P_SEND        -> normal "Bhej do" (UPI id ya phone se)
//     QR_PAY          -> dukaan ka QR scan karke pay kiya
//     REQUEST_MONEY   -> paise MAANGE (abhi tak paisa hila nahi)
//     REQUEST_FULFILL -> maangi hui request ko BHAR diya (yahan paisa hilta hai)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Paisa to sabme same tarah hilta hai — phir type ka fayda kya?         │
//  │                                                                          │
//  │  1. HISTORY me user ko context milta hai — "QR_PAY 120 -> teastall" se   │
//  │     yaad aata hai ki chai ki dukaan pe scan kiya tha.                    │
//  │  2. ANALYTICS/FRAUD: QR payments ka pattern P2P se alag hota hai. Rules  │
//  │     type ke hisaab se lagte hain (jaise QR pe limit alag).               │
//  │  3. AAGE ke rules: kal ko "QR_PAY pe cashback" ya "REQUEST_FULFILL pe    │
//  │     PIN maaf" jaisa rule chahiye to ye field pehle se maujood hai.       │
//  │                                                                          │
//  │  📌 Yaani ye field ABHI ka behavior nahi badalta, par transaction ka     │
//  │     INTENT record karta hai — jo baad me bahut kaam aata hai.            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ Note: `REQUEST_MONEY` abhi kisi Transaction pe set nahi hota — request
//     banne pe sirf `MoneyRequest` object banta hai, Transaction nahi (kyunki
//     paisa hila hi nahi). Paisa tab hilta hai jab payer fulfill karta hai, aur
//     tab type `REQUEST_FULFILL` hota hai. Value future/completeness ke liye hai.
// ============================================================================
#ifndef GPAY_LLD_ENUMS_TRANSACTIONTYPE_H
#define GPAY_LLD_ENUMS_TRANSACTIONTYPE_H

namespace gpay_lld {

enum class TransactionType {
    P2P_SEND,         // normal send (UPI id / phone)
    QR_PAY,           // QR scan karke merchant ko
    REQUEST_MONEY,    // ⚠ abhi unused (request pe Transaction banti hi nahi)
    REQUEST_FULFILL   // maangi hui request bhar di -> yahan paisa hilta hai
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_ENUMS_TRANSACTIONTYPE_H
