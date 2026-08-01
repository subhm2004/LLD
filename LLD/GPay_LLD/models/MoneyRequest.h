// ============================================================================
//  models/MoneyRequest.h  —  "Paise bhej do" wali request (collect flow)
// ----------------------------------------------------------------------------
//  UPI ka COLLECT flow: A, B se paise MAANGTA hai. Request banti hai, par paisa
//  tab tak nahi hilta jab tak B khud APPROVE na kare.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ MoneyRequest ek Transaction NAHI hai — aur yahi poora point hai      │
//  │                                                                          │
//  │     MoneyRequest -> ek IRAADA / guzarish. Paisa NAHI hila.               │
//  │     Transaction  -> paisa ASAL me hila.                                  │
//  │                                                                          │
//  │  Agar request banate hi ek Transaction bhi bana dete, to ledger me aisi   │
//  │  entries bhar jaatin jinme kabhi paisa hila hi nahi. Statement galat,     │
//  │  reconciliation barbaad.                                                 │
//  │                                                                          │
//  │  Isi liye do alag objects hain, aur ye do-step flow banta hai:            │
//  │     createRequest()  -> MoneyRequest (PENDING)   [paisa nahi hila]        │
//  │     fulfillRequest() -> Transaction  (SUCCESS)   [ab paisa hila] +        │
//  │                         request PENDING -> SUCCESS                        │
//  │                                                                          │
//  │  ⭐ SECURITY: paisa PAYER ki taraf se katta hai, aur PAYER hi fulfill      │
//  │     kar sakta hai. Warna koi bhi kisi se paise "kheench" leta. GPaySystem │
//  │     me check hai: `request.getPayerUserId() != payerUserId -> throw`.    │
//  │     (Asli UPI me fraud ka sabse bada raasta yahi collect request hai —    │
//  │      isliye approval hamesha payer ke device pe hota hai.)               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Default status PENDING hai (Transaction ka INITIATED tha) — request banne
//     ka matlab hi hai "jawab ka intezaar".
//
//  ⭐ `isPending()` sirf convenience nahi — ye REPLAY GUARD hai. Service me
//     `if (!isPending()) throw "already processed"` — isse ek hi request DO baar
//     fulfill nahi ho sakti (double payment ka bachaav). 📌 Idempotency ka
//     state-machine wala roop.
// ============================================================================
#ifndef GPAY_LLD_MODELS_MONEYREQUEST_H
#define GPAY_LLD_MODELS_MONEYREQUEST_H

#include <string>
#include <utility>

#include "../enums/TransactionStatus.h"

namespace gpay_lld {

class MoneyRequest {
public:
    MoneyRequest() = default;

    MoneyRequest(std::string requestId, std::string requesterUserId, std::string payerUserId,
                 double amount, std::string note)
        : requestId_(std::move(requestId)),
          requesterUserId_(std::move(requesterUserId)), // jisne MAANGA (paisa isko milega)
          payerUserId_(std::move(payerUserId)),         // jisse maanga (⭐ sirf yahi approve kar sakta)
          amount_(amount),
          note_(std::move(note)) {}

    const std::string& getRequestId() const { return requestId_; }
    const std::string& getRequesterUserId() const { return requesterUserId_; }
    const std::string& getPayerUserId() const { return payerUserId_; }
    double getAmount() const { return amount_; }
    const std::string& getNote() const { return note_; }
    TransactionStatus getStatus() const { return status_; }

    void markFulfilled() { status_ = TransactionStatus::SUCCESS; }
    void markCancelled() { status_ = TransactionStatus::FAILED; }

    // ⭐ REPLAY GUARD: ek request sirf EK baar fulfill ho sakti hai.
    bool isPending() const { return status_ == TransactionStatus::PENDING; }

private:
    std::string requestId_;
    std::string requesterUserId_;
    std::string payerUserId_;
    double amount_{0};
    std::string note_;
    TransactionStatus status_{TransactionStatus::PENDING}; // ⭐ intezaar
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_MODELS_MONEYREQUEST_H
