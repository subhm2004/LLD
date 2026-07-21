// ============================================================================
//  services/TransferService.h  —  ⭐ POORE SYSTEM KA DIL (paisa yahan hilta hai)
// ----------------------------------------------------------------------------
//  Har payment flow (P2P, QR, request-fulfill) ghoom-phir ke ISI method pe aata
//  hai. Ek hi jagah paisa hilta hai — isliye saare guards bhi ek hi jagah hain.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ TRANSFER KA POORA FLOW                                              │
//  │                                                                          │
//  │   1. VALIDATE     -> khud ko nahi bhej sakte, amount > 0 ho              │
//  │   2. IDEMPOTENCY  -> clientRequestId pehle se use hua? (guard #1)        │
//  │   3. DAILY LIMIT  -> aaj ka kota bacha hai? record kar lo (guard #2)     │
//  │   4. TXN BANAO    -> status INITIATED (paisa abhi hila nahi)             │
//  │   5. DEBIT        -> payer se kaato                                      │
//  │   6. CREDIT       -> payee ko do                                         │
//  │   7. SUCCESS      -> markSuccess + ledger me save                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  🐛🐛 BUG FIX #1 (SABSE BADA): debit ho gaya, credit fail -> PAISA GAYAB  │
//  │                                                                          │
//  │  PEHLE code aisa tha:                                                    │
//  │      try {                                                               │
//  │          strategy->debit(payer, ...);                                     │
//  │          strategy->credit(payee, ...);   // <- ye throw kar sakta hai!   │
//  │          txn.markSuccess();                                              │
//  │      } catch (...) { txn.markFailed(); ledger_.save(txn); throw; }        │
//  │                                                                          │
//  │  Agar `credit` fail hua, to catch sirf txn ko FAILED mark karta tha —     │
//  │  par payer ke paise ka DEBIT WAPAS NAHI HOTA THA. Paisa payer se kat      │
//  │  chuka, payee ko mila nahi. Beech me GAYAB. 😱😱                          │
//  │                                                                          │
//  │  Kab hota hai ye? Payee ne bank account link hi na kiya ho —              │
//  │  `BankAccountRailStrategy::credit` "no linked bank account" throw karta   │
//  │  hai. Registration pe wallet to banta hai, bank account NAHI.             │
//  │  CONFIRM kiya tha: Alice 5000 -> Bob (no bank) 250 bheja. Transfer FAIL,  │
//  │  par Alice ka balance 5000 se 4750 ho gaya. ₹250 hawa me. ✅ bug          │
//  │                                                                          │
//  │  FIX: ab debit aur credit ALAG try-blocks me hain. Credit fail hone pe    │
//  │  COMPENSATING TRANSACTION chalta hai — payer ko paisa WAPAS credit.       │
//  │  📌 In-memory bhi ho ya distributed system, jab ek "atomic" kaam do steps │
//  │     me hota hai to har step ka UNDO likhna padta hai. Isi ko compensating │
//  │     transaction / saga kehte hain.                                       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  🐛 BUG FIX #2: fail hone pe bhi DAILY LIMIT kat jaata tha                │
//  │  `recordSpend` payment se pehle hota hai (sahi hai — pre-check), par      │
//  │  failure pe release nahi hota tha. Result: ₹10 balance wala banda 21 baar │
//  │  fail hone ke baad "daily limit exceeded" dekhta tha — bina ek paisa      │
//  │  bheje! Ab har failure path pe `releaseSpend`. (DailyLimitService.h)     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  🐛 BUG FIX #3: fail hone pe clientRequestId "JAL" jaata tha              │
//  │  Id transfer se PEHLE insert hoti thi aur failure pe hatti nahi thi.      │
//  │  Matlab pehli koshish fail (balance kam), user ne paise daale aur SAME    │
//  │  id se dobara try kiya -> "duplicate clientRequestId" 😤 — jabki koi      │
//  │  transfer hua hi nahi tha! Ab failure pe id `erase` ho jaati hai, to      │
//  │  genuine retry chalti hai. SUCCESS ke baad hi id permanently blocked.    │
//  │                                                                          │
//  │  ⚠ Note: ye "reject duplicate" wala model hai, TRUE idempotency nahi.     │
//  │     True idempotency me duplicate pe purana RESULT wapas milta (jaise     │
//  │     Ecommerce checkout me hota hai). Yahan error milta hai — jo UPI apps  │
//  │     me chalta hai kyunki client naya id generate kar leta hai.           │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ GUARDS KA ORDER: sasta check pehle, mehnga baad me.
//     self-transfer/amount (free) -> idempotency (set lookup) -> daily limit
//     -> phir jaake asli paisa. Fail hona hai to jaldi aur sasta fail ho.
//
//  ⚠ Reversal khud fail ho jaye to? Neeche nested catch me handle hai — hum
//     transaction FAILED mark karke aage badh jaate hain, par paisa atka reh
//     jaata hai. Real system me ye ek RECONCILIATION QUEUE me jaata hai
//     (alert + manual/automated settlement). In-memory demo me isse aage nahi
//     ja sakte — par ye jaanna aur bolna hi asli samajh hai.
// ============================================================================
#ifndef GPAY_LLD_SERVICES_TRANSFERSERVICE_H
#define GPAY_LLD_SERVICES_TRANSFERSERVICE_H

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../enums/PaymentRail.h"
#include "../enums/TransactionType.h"
#include "../factories/PaymentRailFactory.h"
#include "../factories/TransactionFactory.h"
#include "../models/BankAccount.h"
#include "../models/Transaction.h"
#include "../models/User.h"
#include "../models/Wallet.h"
#include "../strategies/IPaymentRailStrategy.h"
#include "DailyLimitService.h"
#include "TransactionLedgerService.h"

namespace gpay_lld {

class TransferService {
public:
    TransferService(TransactionLedgerService& ledger, DailyLimitService& limitService)
        : ledger_(ledger), limitService_(limitService) {}

    Transaction execute(const User& payer, const User& payee, double amount, PaymentRail rail,
                        TransactionType type, const std::string& note,
                        std::unordered_map<std::string, BankAccount>& bankAccounts,
                        std::unordered_map<std::string, Wallet>& wallets,
                        const std::unordered_map<std::string, std::string>& primaryAccountByUser,
                        std::unordered_set<std::string>& processedClientIds,
                        const std::string& clientRequestId, int& txnCounter) {
        // ---- STEP 1: sabse saste checks pehle (abhi kuch reserve nahi hua) ----
        if (payer.getUserId() == payee.getUserId()) {
            throw std::runtime_error("cannot transfer to self");
        }
        if (amount <= 0) {
            throw std::invalid_argument("amount must be positive");
        }

        // ---- STEP 2: GUARD #1 — duplicate request? -------------------------
        if (!clientRequestId.empty()) {
            if (processedClientIds.count(clientRequestId) != 0) {
                throw std::runtime_error("duplicate clientRequestId");
            }
            processedClientIds.insert(clientRequestId); // ⚠ fail hone pe hatana ZAROORI (fix #3)
        }

        // ---- STEP 3: GUARD #2 — daily limit (check + record ek saath) -------
        try {
            limitService_.recordSpend(payer.getUserId(), amount);
        } catch (...) {
            // Limit hi fail -> abhi tak sirf clientId reserve hui thi, use chhod do.
            releaseClientId(processedClientIds, clientRequestId);
            throw;
        }

        // ---- STEP 4: Transaction banao (INITIATED — paisa abhi hila nahi) ----
        const std::string txnId = "TXN_" + std::to_string(++txnCounter);
        Transaction txn = TransactionFactory::create(
            txnId, payer.getUserId(), payee.getUserId(), payer.getUpiId(), payee.getUpiId(),
            amount, type, rail, note);

        // Rail ke hisaab se strategy (bank / wallet) — Factory + Strategy.
        std::shared_ptr<IPaymentRailStrategy> strategy =
            PaymentRailFactory::create(rail, primaryAccountByUser);

        // ---- STEP 5: DEBIT — payer se paisa kaato ---------------------------
        try {
            strategy->debit(payer.getUserId(), amount, bankAccounts, wallets);
        } catch (...) {
            // ✅ Yahan paisa hila hi nahi (debit hi fail hua) — sirf guards chhodo.
            rollbackGuards(payer.getUserId(), amount, processedClientIds, clientRequestId);
            txn.markFailed();
            ledger_.save(txn); // ⭐ failure ka record bhi rehta hai
            throw;
        }

        // ---- STEP 6: CREDIT — payee ko paisa do ----------------------------
        try {
            strategy->credit(payee.getUserId(), amount, bankAccounts, wallets);
        } catch (...) {
            // 🐛🐛 FIX #1: paisa payer se KAT CHUKA hai — usko WAPAS karo!
            // Bina iske ₹ hawa me gayab ho jaate the (upar poora note).
            try {
                strategy->credit(payer.getUserId(), amount, bankAccounts, wallets);
            } catch (...) {
                // ⚠ Reversal bhi fail — real system me ye reconciliation queue
                // me jaata hai (alert + manual settlement). Yahan bas FAILED
                // mark karke aage badhte hain; exception nigalte nahi, kyunki
                // asli wajah (credit ka failure) neeche rethrow hogi.
            }
            rollbackGuards(payer.getUserId(), amount, processedClientIds, clientRequestId);
            txn.markFailed();
            ledger_.save(txn);
            throw; // original credit-failure ko upar bhejo (bare rethrow)
        }

        // ---- STEP 7: dono ho gaye -> SUCCESS -------------------------------
        // ⭐ markSuccess() sirf yahan — jab debit AUR credit dono confirm ho chuke.
        txn.markSuccess();
        ledger_.save(txn);
        return txn;
    }

private:
    TransactionLedgerService& ledger_;
    DailyLimitService& limitService_;

    // clientRequestId ko free karo taaki GENUINE retry block na ho (fix #3).
    static void releaseClientId(std::unordered_set<std::string>& processedClientIds,
                                const std::string& clientRequestId) {
        if (!clientRequestId.empty()) {
            processedClientIds.erase(clientRequestId);
        }
    }

    // ⭐ Dono pre-reserved guards ek saath chhodo — har failure path pe yahi.
    void rollbackGuards(const std::string& payerUserId, double amount,
                        std::unordered_set<std::string>& processedClientIds,
                        const std::string& clientRequestId) {
        limitService_.releaseSpend(payerUserId, amount); // fix #2
        releaseClientId(processedClientIds, clientRequestId); // fix #3
    }
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_SERVICES_TRANSFERSERVICE_H
