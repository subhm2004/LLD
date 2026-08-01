// ============================================================================
//  factories/TransactionFactory.h  —  Transaction banane ka ek hi darwaza
// ----------------------------------------------------------------------------
//  Ye "Strategy banane wali" factory nahi hai — ye SIMPLE FACTORY hai jo ek hi
//  tarah ka object banata hai, par usme ek RULE fix kar deta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Iska asli kaam: status HAMESHA `INITIATED` se shuru ho                │
//  │                                                                          │
//  │  Dekho — caller `status` param deta hi NAHI. Factory khud                 │
//  │  `TransactionStatus::INITIATED` bhar deta hai.                            │
//  │                                                                          │
//  │  Kyun zaroori hai? Kyunki agar caller status de sakta, to koi bhi galti   │
//  │  se (ya jaan-boojh ke) seedha `SUCCESS` wali transaction bana deta —      │
//  │  BINA paisa hile! Ledger jhooth bolne lagta. 😱                           │
//  │                                                                          │
//  │  Ab rule hard-coded hai: har transaction "shuru hui" state me paida hoti  │
//  │  hai, aur SUCCESS sirf `markSuccess()` se aata hai — jo TransferService   │
//  │  tabhi bulata hai jab debit+credit dono ho chuke hon.                     │
//  │                                                                          │
//  │  📌 Ye Factory ka wo use hai jo log miss karte hain: sirf "object banana" │
//  │     nahi, balki "object hamesha SAHI initial state me bane" — ek          │
//  │     INVARIANT ko ek jagah pe lock karna.                                  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Fayda #2: agar kal ko har transaction me timestamp, deviceId ya
//     geo-location add karna ho — sirf YAHAN ek line. Saare call sites apne aap
//     ussi format me transaction banayenge. Bina factory ke har `new Transaction`
//     wali jagah dhundh ke badalni padti.
//
//  📌 Return by VALUE hai (pointer nahi) — Transaction ek chhota data object hai,
//     ownership ka koi jhamela nahi. Move semantics se copy ki tension bhi nahi.
// ============================================================================
#ifndef GPAY_LLD_FACTORIES_TRANSACTIONFACTORY_H
#define GPAY_LLD_FACTORIES_TRANSACTIONFACTORY_H

#include <string>

#include "../enums/PaymentRail.h"
#include "../enums/TransactionStatus.h"
#include "../enums/TransactionType.h"
#include "../models/Transaction.h"

namespace gpay_lld {

class TransactionFactory {
public:
    static Transaction create(const std::string& txnId, const std::string& payerUserId,
                              const std::string& payeeUserId, const std::string& payerUpi,
                              const std::string& payeeUpi, double amount, TransactionType type,
                              PaymentRail rail, const std::string& note = "") {
        // ⭐ status caller se NAHI aata — hamesha INITIATED (upar note).
        return Transaction(txnId, payerUserId, payeeUserId, payerUpi, payeeUpi, amount, type,
                           TransactionStatus::INITIATED, rail, note);
    }
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_FACTORIES_TRANSACTIONFACTORY_H
