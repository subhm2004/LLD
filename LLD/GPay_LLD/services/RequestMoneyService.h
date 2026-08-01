// ============================================================================
//  services/RequestMoneyService.h  —  "Paise bhej do" requests ka registry
// ----------------------------------------------------------------------------
//  UPI ka COLLECT flow. Yaad rakho: ye service paisa BILKUL nahi hilati — wo
//  TransferService ka kaam hai. Ye sirf requests ka record rakhti hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Do-step flow (aur wo alag kyun hai)                                   │
//  │                                                                          │
//  │     createRequest()  -> MoneyRequest banti hai, status PENDING            │
//  │                         [paisa NAHI hila]                                │
//  │     ...payer approve karta hai...                                        │
//  │     markFulfilled()  -> status SUCCESS                                    │
//  │                         [paisa TransferService ne hilaya]                │
//  │                                                                          │
//  │  Request banate hi paisa nahi khichta — warna koi bhi kisi se paise       │
//  │  "kheench" leta. Approval hamesha PAYER ki taraf se hoti hai.            │
//  │  (Ye check GPaySystem me hai: `request.getPayerUserId() != payerUserId`.) │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ `markFulfilled` ka `isPending()` check = DOUBLE-PAY GUARD           │
//  │                                                                          │
//  │      if (!request.isPending()) throw "request already processed";        │
//  │                                                                          │
//  │  Bina iske: payer galti se (ya double-tap se) ek hi request DO baar       │
//  │  fulfill kar deta -> paisa DO baar jaata. 😱 Status hi yahan lock ka kaam │
//  │  karta hai — ek baar SUCCESS/FAILED ho gaya to dobara nahi ho sakta.      │
//  │  📌 Ye idempotency ka STATE-MACHINE wala roop hai (Ecommerce me           │
//  │     clientRequestId wala roop tha) — dono ka maqsad ek: DUPLICATE ROKO.  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ ORDER KA NOTE (GPaySystem::fulfillMoneyRequest me): pehle transfer hota
//     hai, PHIR `markFulfilled`. Yaani transfer fail hua to request PENDING hi
//     rehti hai — payer dobara try kar sakta hai. ✅ Sahi order.
//     Ulta karte (pehle mark, phir transfer) to fail hone pe request "fulfilled"
//     dikhti par paisa gaya hi nahi hota. 😱
//
//  ⭐ `createRequest` me `requester == payer` block hai — khud se paise maangna
//     bekaar hai. (TransferService me bhi self-transfer block hai — do layer.)
//
//  📌 `getRequestOrThrow` NON-const reference deta hai kyunki caller ko request
//     ki state badalni hoti hai (fulfill/cancel). Naam me hi "OrThrow" hai —
//     yahan "nahi mila" ek asli error hai, normal case nahi.
// ============================================================================
#ifndef GPAY_LLD_SERVICES_REQUESTMONEYSERVICE_H
#define GPAY_LLD_SERVICES_REQUESTMONEYSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../models/MoneyRequest.h"
#include "../models/User.h"

namespace gpay_lld {

class RequestMoneyService {
public:
    // Request banao — paisa yahan BILKUL nahi hilta (sirf record).
    std::string createRequest(const User& requester, const User& payer, double amount,
                              const std::string& note, int& requestCounter) {
        if (requester.getUserId() == payer.getUserId()) {
            throw std::runtime_error("cannot request money from self");
        }
        if (amount <= 0) {
            throw std::invalid_argument("amount must be positive");
        }
        const std::string requestId = "REQ_" + std::to_string(++requestCounter);
        requests_.emplace(requestId,
                          MoneyRequest(requestId, requester.getUserId(), payer.getUserId(),
                                       amount, note)); // status PENDING se shuru
        return requestId;
    }

    MoneyRequest& getRequestOrThrow(const std::string& requestId) {
        auto it = requests_.find(requestId);
        if (it == requests_.end()) {
            throw std::runtime_error("money request not found");
        }
        return it->second;
    }

    // ⭐⭐ DOUBLE-PAY GUARD yahi hai (upar note).
    void markFulfilled(const std::string& requestId) {
        MoneyRequest& request = getRequestOrThrow(requestId);
        if (!request.isPending()) {
            throw std::runtime_error("request already processed");
        }
        request.markFulfilled();
    }

private:
    std::unordered_map<std::string, MoneyRequest> requests_; // requestId -> request
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_SERVICES_REQUESTMONEYSERVICE_H
