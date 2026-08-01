// ============================================================================
//  core/GPaySystem.h  —  FACADE (poore GPay ka ek hi darwaza)
// ----------------------------------------------------------------------------
//  Andar 5 services + 2 factories + 2 strategies hain. Bahar (main.cpp / API
//  layer) ko unme se ek ka bhi naam nahi pata — bas `gpay.sendMoney(...)`.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Facade ka kaam (aur jo NAHI hai)                                      │
//  │                                                                          │
//  │   KARTA HAI:                                                             │
//  │     - saara DATA own karta hai (users, accounts, wallets, lookups)        │
//  │     - services ko own + wire karta hai                                    │
//  │     - identity resolution: UPI/phone -> user                              │
//  │     - AUTHORIZATION: PIN check, "request ka payer tu hi hai na?"          │
//  │     - id counters (userCounter_, txnCounter_, ...)                        │
//  │                                                                          │
//  │   NAHI KARTA:                                                            │
//  │     - paisa hilana (TransferService), limit ka hisaab (DailyLimitService),│
//  │       PIN compare (PinAuthService), account dhundna (Strategy)           │
//  │  📌 Facade "coordinator" hota hai, "God object" nahi.                    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ TEEN LOOKUP MAPS — O(1) resolution ki keemat                        │
//  │                                                                          │
//  │     users_          : userId -> User        (asli data)                  │
//  │     upiToUserId_    : upiId  -> userId      (index)                      │
//  │     phoneToUserId_  : phone  -> userId      (index)                      │
//  │                                                                          │
//  │  Do extra maps sirf isliye ki "alice@okaxis kaun hai?" O(1) me pata chale │
//  │  (warna har payment pe saare users scan karne padte).                    │
//  │  Ye DB ke SECONDARY INDEX ka in-memory roop hai.                         │
//  │                                                                          │
//  │  ⭐ Aur ye maps DUPLICATE ROKNE ka kaam bhi karte hain: registration me   │
//  │     `upiToUserId_.count(upiId)` se pata chal jaata hai ki VPA le liya     │
//  │     gaya hai. Index + unique-constraint dono ek saath.                   │
//  │  ⚠ Keemat: teeno maps ko SYNC me rakhna padta hai. Ek jagah insert bhoole │
//  │     to system silently toot jaata. Isi liye writes ek hi method           │
//  │     (`registerUser`) me hain — bikhre hue nahi.                          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠⚠ BUG FIX: copy/move `= delete` — dangling reference se bachaav        │
//  │                                                                          │
//  │  PEHLE ye class implicitly COPYABLE aur MOVABLE thi. Test karke confirm   │
//  │  kiya tha ki ye line compile ho jaati thi:                                │
//  │      GPaySystem g;  GPaySystem h = std::move(g);   // ✅ compile 😱       │
//  │                                                                          │
//  │  PROBLEM: `transferService_` andar `ledger_` aur `limitService_` ke        │
//  │  REFERENCES rakhta hai — dono SIBLING MEMBERS hain (isi object ke).       │
//  │  Copy/move memberwise hota hai: references COPY hoti hain, RE-BIND NAHI.  │
//  │  Matlab `h.transferService_` abhi bhi `g.ledger_` ko point karega:        │
//  │      h.sendMoney(...) -> transaction 'g' ke ledger me save hoga,          │
//  │                          aur 'h' ki history hamesha khaali dikhegi 🤯     │
//  │      'g' scope se bahar -> 'h' ki references DANGLING -> use-after-free   │
//  │                                                                          │
//  │  ⭐ ASLI WAJAH: C++ ka default copy/move ko pata hi nahi hota ki members   │
//  │     aapas me jude hue hain (self-referential).                           │
//  │                                                                          │
//  │  FIX: chaaron `= delete`. Ab galti se copy/move likha to COMPILE ERROR —  │
//  │  runtime pe paisa idhar-udhar hone ka mauka hi nahi.                     │
//  │  📌 RULE: jis class ke members ek dusre ko reference karte hon, wo class  │
//  │     na copy honi chahiye na move.                                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ MEMBER ORDER MATTERS: `ledger_` aur `limitService_` `transferService_` se
//     PEHLE declare hain — kyunki C++ me members DECLARATION ke order me bante
//     hain (init-list ke order me nahi). Ulta hota to transferService_ un
//     members ka reference leta jo abhi bane hi nahi = UB. 😱
// ============================================================================
#ifndef GPAY_LLD_CORE_GPAYSYSTEM_H
#define GPAY_LLD_CORE_GPAYSYSTEM_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../enums/PaymentRail.h"
#include "../enums/TransactionType.h"
#include "../models/BankAccount.h"
#include "../models/Beneficiary.h"
#include "../models/MoneyRequest.h"
#include "../models/Transaction.h"
#include "../models/User.h"
#include "../models/Wallet.h"
#include "../services/DailyLimitService.h"
#include "../services/PinAuthService.h"
#include "../services/RequestMoneyService.h"
#include "../services/TransactionLedgerService.h"
#include "../services/TransferService.h"
#include "../utils/UpiUtils.h"

namespace gpay_lld {

class GPaySystem {
public:
    // Wiring point: transferService_ ko ledger + limit ke references milte hain.
    GPaySystem() : transferService_(ledger_, limitService_) {}

    // ⚠⚠ BUG FIX: transferService_ sibling members ko reference karta hai,
    // isliye copy/move dono BANNED (upar poora note).
    GPaySystem(const GPaySystem&) = delete;
    GPaySystem& operator=(const GPaySystem&) = delete;
    GPaySystem(GPaySystem&&) = delete;
    GPaySystem& operator=(GPaySystem&&) = delete;

    // ---- Onboarding --------------------------------------------------------
    std::string registerUser(const std::string& name, const std::string& phone,
                             const std::string& upiId, const std::string& pin) {
        validateUpiId(upiId);
        const std::string normalizedPhone = normalizePhone(phone); // ⭐ ek hi format

        // ⭐ Uniqueness check — index maps hi constraint ka kaam karte hain.
        if (upiToUserId_.count(upiId) != 0) {
            throw std::runtime_error("UPI ID already taken: " + upiId);
        }
        if (phoneToUserId_.count(normalizedPhone) != 0) {
            throw std::runtime_error("phone already registered");
        }

        const std::string userId = "GP_U" + std::to_string(++userCounter_);
        // ⭐ PIN ka HASH store hota hai, PIN nahi (PinAuthService.h ka note).
        User user(userId, name, normalizedPhone, upiId, PinAuthService::hashPin(pin));
        users_.emplace(userId, user);
        upiToUserId_[upiId] = userId;            // index #1
        phoneToUserId_[normalizedPhone] = userId; // index #2
        wallets_.emplace(userId, Wallet(userId, 0)); // har user ka wallet turant
        return userId;
    }

    std::string linkBankAccount(const std::string& userId, const std::string& bankName,
                                const std::string& maskedNumber, double initialBalance) {
        User& user = getUserOrThrow(userId);
        const std::string accountId = "ACC_" + std::to_string(++accountCounter_);
        bankAccounts_.emplace(accountId,
                              BankAccount(accountId, user.getUserId(), bankName, maskedNumber,
                                          initialBalance));
        // ⭐ PEHLA linked account apne aap PRIMARY ban jaata hai. Ye map hi
        // authority hai — debit/credit isi se account chunta hai
        // (BankAccountRailStrategy ko ye map pass hota hai).
        if (primaryAccountByUser_.count(userId) == 0) {
            primaryAccountByUser_[userId] = accountId;
        }
        return accountId;
    }

    // ---- Balances ----------------------------------------------------------
    double getWalletBalance(const std::string& userId) const {
        return getWalletOrThrow(userId).getBalance();
    }

    void topUpWallet(const std::string& userId, double amount) {
        validateUser(userId);
        if (amount <= 0) {
            throw std::invalid_argument("top-up amount must be positive");
        }
        wallets_.at(userId).credit(amount);
    }

    double getPrimaryBankBalance(const std::string& userId) const {
        const BankAccount& account = getPrimaryBankOrThrow(userId);
        return account.getBalance();
    }

    // Aaj kitna aur bhej sakte ho (daily limit ka bacha hua hissa).
    double getRemainingDailyLimit(const std::string& userId) const {
        validateUser(userId);
        return limitService_.getRemaining(userId);
    }

    // ---- Beneficiaries (saved contacts) ------------------------------------
    void addBeneficiary(const std::string& userId, const std::string& nickname,
                        const std::string& upiId) {
        validateUser(userId);
        // ⭐ Sirf format validate — VPA GPay pe registered ho ye zaroori nahi
        // (UPI open network hai — Beneficiary.h ka note).
        validateUpiId(upiId);
        beneficiaries_[userId].emplace_back(userId, nickname, upiId);
    }

    std::vector<Beneficiary> listBeneficiaries(const std::string& userId) const {
        validateUser(userId);
        auto it = beneficiaries_.find(userId);
        if (it == beneficiaries_.end()) {
            return {}; // koi contact save nahi — khaali list, error nahi
        }
        return it->second;
    }

    // ---- ⭐ Payment flows (teeno ek hi TransferService pe milte hain) -------

    // (1) Normal P2P — UPI ID pe bhejo.
    Transaction sendMoney(const std::string& payerUserId, const std::string& payeeUpiId,
                          double amount, const std::string& pin, PaymentRail rail,
                          const std::string& note = "",
                          const std::string& clientRequestId = "") {
        User& payer = getUserOrThrow(payerUserId);
        pinAuth_.verifyPin(payer, pin);              // ⭐ AUTH pehle
        const User& payee = resolveUserByUpi(payeeUpiId); // identity resolution
        return transferService_.execute(payer, payee, amount, rail, TransactionType::P2P_SEND,
                                        note, bankAccounts_, wallets_, primaryAccountByUser_,
                                        processedClientIds_, clientRequestId, txnCounter_);
    }

    // (2) Phone se bhejo — bas phone ko UPI me badal ke sendMoney call.
    // ⭐ Logic DUPLICATE nahi kiya — resolution karke wahi method reuse.
    Transaction sendMoneyToPhone(const std::string& payerUserId, const std::string& payeePhone,
                                 double amount, const std::string& pin, PaymentRail rail,
                                 const std::string& note = "") {
        const std::string phone = normalizePhone(payeePhone); // ⭐ normalize (warna miss)
        auto it = phoneToUserId_.find(phone);
        if (it == phoneToUserId_.end()) {
            throw std::runtime_error("payee phone not registered on GPay");
        }
        return sendMoney(payerUserId, users_.at(it->second).getUpiId(), amount, pin, rail, note);
    }

    // (3) QR scan karke pay.
    Transaction scanAndPay(const std::string& payerUserId, const std::string& qrPayload,
                           const std::string& pin, PaymentRail rail) {
        User& payer = getUserOrThrow(payerUserId);
        pinAuth_.verifyPin(payer, pin);
        // ⭐ QR bahar ka input hai — parse + validate (UpiUtils me hota hai).
        QrPaymentData qr = parseUpiQr(qrPayload);
        const User& payee = resolveUserByUpi(qr.payeeUpi);
        // ⭐ Amount QR se aata hai, user se nahi — merchant ne fix kiya hua hai.
        return transferService_.execute(payer, payee, qr.amount, rail, TransactionType::QR_PAY,
                                        qr.note, bankAccounts_, wallets_, primaryAccountByUser_,
                                        processedClientIds_, "", txnCounter_);
    }

    // ---- Collect flow (request -> fulfill) ---------------------------------
    std::string requestMoney(const std::string& requesterUserId, const std::string& payerUpiId,
                             double amount, const std::string& note) {
        const User& requester = getUserOrThrow(requesterUserId);
        const User& payer = resolveUserByUpi(payerUpiId);
        // ⭐ Paisa yahan BILKUL nahi hilta — sirf request record banti hai.
        return requestMoneyService_.createRequest(requester, payer, amount, note, requestCounter_);
    }

    Transaction fulfillMoneyRequest(const std::string& payerUserId, const std::string& requestId,
                                    const std::string& pin, PaymentRail rail) {
        User& payer = getUserOrThrow(payerUserId);
        pinAuth_.verifyPin(payer, pin);

        MoneyRequest& request = requestMoneyService_.getRequestOrThrow(requestId);
        // ⭐⭐ SECURITY: sirf wahi bhar sakta hai jisse maanga gaya tha. Bina is
        // check ke koi bhi kisi aur ki request "approve" kar deta = paisa churana.
        if (request.getPayerUserId() != payerUserId) {
            throw std::runtime_error("only payer can fulfill this request");
        }
        const User& requester = getUserOrThrow(request.getRequesterUserId());

        // ⭐ ORDER: pehle TRANSFER, phir markFulfilled. Transfer fail hua to
        // request PENDING hi rehti hai -> payer dobara try kar sakta hai. ✅
        Transaction txn = transferService_.execute(
            payer, requester, request.getAmount(), rail, TransactionType::REQUEST_FULFILL,
            request.getNote(), bankAccounts_, wallets_, primaryAccountByUser_,
            processedClientIds_, "", txnCounter_);
        requestMoneyService_.markFulfilled(requestId); // double-pay guard andar hai
        return txn;
    }

    // ---- History -----------------------------------------------------------
    std::vector<Transaction> getTransactionHistory(const std::string& userId,
                                                   size_t limit = 20) const {
        validateUser(userId);
        return ledger_.listForUser(userId, limit); // sent + received dono
    }

    const User& getUserByUpi(const std::string& upiId) const { return resolveUserByUpi(upiId); }

private:
    // ---- Data (facade hi maalik hai) ---------------------------------------
    std::unordered_map<std::string, User> users_;                 // userId -> user
    std::unordered_map<std::string, std::string> upiToUserId_;    // ⭐ index + unique constraint
    std::unordered_map<std::string, std::string> phoneToUserId_;  // ⭐ index + unique constraint
    std::unordered_map<std::string, BankAccount> bankAccounts_;   // accountId -> account
    std::unordered_map<std::string, std::string> primaryAccountByUser_; // ⭐ debit/credit ki authority
    std::unordered_map<std::string, Wallet> wallets_;             // userId -> wallet (1:1)
    std::unordered_map<std::string, std::vector<Beneficiary>> beneficiaries_;
    std::unordered_set<std::string> processedClientIds_;          // duplicate-request guard

    // ---- Services (⭐ ORDER MATTERS — upar note) ---------------------------
    TransactionLedgerService ledger_;
    DailyLimitService limitService_;
    PinAuthService pinAuth_;
    RequestMoneyService requestMoneyService_;
    TransferService transferService_; // <- ledger_ aur limitService_ ke reference leta hai

    int userCounter_{0};
    int accountCounter_{0};
    int txnCounter_{0};
    int requestCounter_{0};

    // ---- Private helpers ---------------------------------------------------
    void validateUser(const std::string& userId) const {
        if (users_.find(userId) == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
    }

    // Non-const: caller ko User modify karne ki chhoot (abhi koi karta nahi).
    User& getUserOrThrow(const std::string& userId) {
        auto it = users_.find(userId);
        if (it == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
        return it->second;
    }

    // ⭐ Const overload — const methods ke liye read-only view (const-correctness).
    const User& getUserOrThrow(const std::string& userId) const {
        auto it = users_.find(userId);
        if (it == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
        return it->second;
    }

    // ⭐ UPI -> User: pehle FORMAT check, phir REGISTRATION check.
    // Do alag errors — "format galat" aur "registered nahi" bilkul alag baatein hain.
    const User& resolveUserByUpi(const std::string& upiId) const {
        validateUpiId(upiId);
        auto it = upiToUserId_.find(upiId);
        if (it == upiToUserId_.end()) {
            throw std::runtime_error("UPI ID not registered: " + upiId);
        }
        return users_.at(it->second); // index -> asli data (O(1))
    }

    const Wallet& getWalletOrThrow(const std::string& userId) const {
        auto it = wallets_.find(userId);
        if (it == wallets_.end()) {
            throw std::runtime_error("wallet not found");
        }
        return it->second;
    }

    // ⭐ Do-step lookup: user -> primary accountId -> account.
    // Wahi map jo strategy bhi use karti hai -> UI ka balance aur debit hone
    // wala account HAMESHA same (ye pehle match nahi karta tha — bug tha).
    const BankAccount& getPrimaryBankOrThrow(const std::string& userId) const {
        auto primaryIt = primaryAccountByUser_.find(userId);
        if (primaryIt == primaryAccountByUser_.end()) {
            throw std::runtime_error("no bank account linked");
        }
        auto accIt = bankAccounts_.find(primaryIt->second);
        if (accIt == bankAccounts_.end()) {
            throw std::runtime_error("bank account missing"); // data inconsistency
        }
        return accIt->second;
    }
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_CORE_GPAYSYSTEM_H
