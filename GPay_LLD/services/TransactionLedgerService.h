// ============================================================================
//  services/TransactionLedgerService.h  —  Transactions ka ledger (repository)
// ----------------------------------------------------------------------------
//  Sab transactions yahin store hote hain — SUCCESS wali bhi aur FAILED wali bhi.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FAILED transactions bhi save hoti hain — ye jaan-boojh ke hai         │
//  │                                                                          │
//  │  Sochne me aata hai "fail hui to save hi kyun karein?" Par:              │
//  │   • User ko dikhna chahiye ki uski koshish FAIL hui thi (warna wo soch    │
//  │     me pad jaata "paisa gaya ya nahi?" aur dobara bhej deta 😱)          │
//  │   • Support team ko debug karne ko record chahiye                        │
//  │   • Fraud detection ke liye failed attempts ka pattern sabse zaroori data │
//  │     hota hai (koi baar-baar galat PIN try kar raha hai?)                 │
//  │                                                                          │
//  │  📌 Ledger "sirf paisa" nahi, "kya-kya HUA" ka record hai.               │
//  │     (TransferService ke catch blocks me `ledger_.save(txn)` dekho.)      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `listForUser` payer AUR payee dono match karta hai                    │
//  │                                                                          │
//  │      t.getPayerUserId() == userId || t.getPayeeUserId() == userId        │
//  │                                                                          │
//  │  Yaani ek hi Transaction object DO logon ki history me dikhta hai —       │
//  │  bhejne wale ko "gaya" aur lene wale ko "aaya". Isi liye alag "sent" aur  │
//  │  "received" tables nahi banaye gaye. Ek entry, do views.                 │
//  │  ⚠ Trade-off: ye O(N) full scan hai. Production me `userId -> txnIds`     │
//  │     wala index (ya DB index) chahiye hoga.                               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Sort DESCENDING hai (`a > b`) — newest transaction sabse upar, bilkul asli
//     app jaisa. TxnId "TXN_1", "TXN_2"... incrementing hai, isliye string sort
//     ≈ time sort. ⚠ Caveat: "TXN_10" < "TXN_9" lexicographically! 10+ txn pe
//     order galat ho jayega — asli fix timestamp field ya zero-padded ids.
//
//  ⭐ Sort ke BAAD `resize(limit)` — pehle resize karte to random 50 uthte aur
//     phir unhe sort karte, yaani "latest 50" nahi milte. Order matter karta hai.
//
//  📌 `findById` pointer deta hai (reference nahi) — kyunki "nahi mila" ek NORMAL
//     case hai, error nahi. `nullptr` se caller decide karta hai. (Jahan "hona
//     hi chahiye" wahan `...OrThrow` naam wale methods hain.)
// ============================================================================
#ifndef GPAY_LLD_SERVICES_TRANSACTIONLEDGERSERVICE_H
#define GPAY_LLD_SERVICES_TRANSACTIONLEDGERSERVICE_H

#include <algorithm>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../models/Transaction.h"

namespace gpay_lld {

class TransactionLedgerService {
public:
    // by-value + move: caller apni transaction "de deta" hai (copy nahi hoti).
    void save(Transaction txn) { transactions_[txn.getTxnId()] = std::move(txn); }

    // Nahi mila -> nullptr (ye normal case hai, error nahi).
    Transaction* findById(const std::string& txnId) {
        auto it = transactions_.find(txnId);
        if (it == transactions_.end()) {
            return nullptr;
        }
        return &it->second;
    }

    // ⭐ Sent + received dono (ek entry, do logon ki history me).
    std::vector<Transaction> listForUser(const std::string& userId, size_t limit = 50) const {
        std::vector<Transaction> result;
        for (const auto& entry : transactions_) { // ⚠ O(N) scan (upar note)
            const Transaction& t = entry.second;
            if (t.getPayerUserId() == userId || t.getPayeeUserId() == userId) {
                result.push_back(t);
            }
        }
        // ⭐ Newest pehle (descending). Sort PEHLE, limit BAAD me.
        std::sort(result.begin(), result.end(),
                  [](const Transaction& a, const Transaction& b) {
                      return a.getTxnId() > b.getTxnId();
                  });
        if (result.size() > limit) {
            result.resize(limit); // ⭐ ab ye sach me "latest N" hain
        }
        return result;
    }

private:
    std::unordered_map<std::string, Transaction> transactions_; // txnId -> txn (ye hi "DB" hai)
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_SERVICES_TRANSACTIONLEDGERSERVICE_H
