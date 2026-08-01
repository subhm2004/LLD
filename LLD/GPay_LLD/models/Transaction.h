// ============================================================================
//  models/Transaction.h  —  Ek payment ka poora record (ledger entry)
// ----------------------------------------------------------------------------
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ userId AUR upiId — DONO store hote hain. Duplicate nahi hai!         │
//  │                                                                          │
//  │     payerUserId_ / payeeUserId_  -> INTERNAL id ("GP_U1")                 │
//  │     payerUpi_    / payeeUpi_     -> UPI ID ("alice@okaxis")               │
//  │                                                                          │
//  │  userId kis liye: history filter, joins, analytics — internal kaam.       │
//  │  upiId kis liye:  ⭐ SNAPSHOT — user ne us waqt JIS VPA pe bheja tha.     │
//  │                                                                          │
//  │  Sirf userId store karte to: user apna UPI ID badal le (allowed hai), aur │
//  │  purani transactions me NAYA VPA dikhne lagta. Statement galat ho jaata,  │
//  │  aur dispute me proof kharab. Isliye VPA ki COPY.                        │
//  │  📌 Yahi wo hi SNAPSHOT soch hai jo invoice/order lines me hoti hai —     │
//  │     historical record kabhi peeche se badalna nahi chahiye.              │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `rail_` bhi store hota hai — paisa BANK se gaya tha ya WALLET se.
//     Statement me dikhana padta hai, aur refund/reversal usi rail pe jaana
//     chahiye jahan se paisa aaya tha.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Sirf 2 mutators: `markSuccess()` / `markFailed()`                     │
//  │                                                                          │
//  │  `setStatus(kuch bhi)` NAHI diya gaya. Iska matlab status sirf INITIATED  │
//  │  se aage hi ja sakta hai — koi galti se SUCCESS ko wapas INITIATED nahi   │
//  │  kar sakta. Amount, payer, payee to bilkul immutable hain (koi setter     │
//  │  hi nahi) — ek baar transaction ban gayi to uska paisa/parties FIX.       │
//  │  📌 Ye "restricted state transitions" hai — enum + limited setters se     │
//  │     ek chhota state machine ban jaata hai.                               │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef GPAY_LLD_MODELS_TRANSACTION_H
#define GPAY_LLD_MODELS_TRANSACTION_H

#include <string>
#include <utility>

#include "../enums/PaymentRail.h"
#include "../enums/TransactionStatus.h"
#include "../enums/TransactionType.h"

namespace gpay_lld {

class Transaction {
public:
    Transaction() = default;

    Transaction(std::string txnId, std::string payerUserId, std::string payeeUserId,
                std::string payerUpi, std::string payeeUpi, double amount,
                TransactionType type, TransactionStatus status, PaymentRail rail,
                std::string note = "")
        : txnId_(std::move(txnId)),
          payerUserId_(std::move(payerUserId)),
          payeeUserId_(std::move(payeeUserId)),
          payerUpi_(std::move(payerUpi)),   // ⭐ VPA ka snapshot
          payeeUpi_(std::move(payeeUpi)),   // ⭐ VPA ka snapshot
          amount_(amount),
          type_(type),
          status_(status),
          rail_(rail),                      // bank se gaya ya wallet se
          note_(std::move(note)) {}

    const std::string& getTxnId() const { return txnId_; }
    const std::string& getPayerUserId() const { return payerUserId_; }
    const std::string& getPayeeUserId() const { return payeeUserId_; }
    const std::string& getPayerUpi() const { return payerUpi_; }
    const std::string& getPayeeUpi() const { return payeeUpi_; }
    double getAmount() const { return amount_; }
    TransactionType getType() const { return type_; }
    TransactionStatus getStatus() const { return status_; }
    PaymentRail getRail() const { return rail_; }
    const std::string& getNote() const { return note_; }

    // ⭐ Sirf ye do — koi general setStatus() nahi (upar note).
    void markSuccess() { status_ = TransactionStatus::SUCCESS; }
    void markFailed() { status_ = TransactionStatus::FAILED; }

private:
    std::string txnId_;
    std::string payerUserId_;  // internal
    std::string payeeUserId_;  // internal
    std::string payerUpi_;     // ⭐ snapshot
    std::string payeeUpi_;     // ⭐ snapshot
    double amount_{0};
    TransactionType type_{TransactionType::P2P_SEND};
    TransactionStatus status_{TransactionStatus::INITIATED}; // ⭐ safe default
    PaymentRail rail_{PaymentRail::LINKED_BANK};
    std::string note_;         // "Lunch", "Chai"
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_MODELS_TRANSACTION_H
