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
    GPaySystem() : transferService_(ledger_, limitService_) {}

    std::string registerUser(const std::string& name, const std::string& phone,
                             const std::string& upiId, const std::string& pin) {
        validateUpiId(upiId);
        const std::string normalizedPhone = normalizePhone(phone);

        if (upiToUserId_.count(upiId) != 0) {
            throw std::runtime_error("UPI ID already taken: " + upiId);
        }
        if (phoneToUserId_.count(normalizedPhone) != 0) {
            throw std::runtime_error("phone already registered");
        }

        const std::string userId = "GP_U" + std::to_string(++userCounter_);
        User user(userId, name, normalizedPhone, upiId, PinAuthService::hashPin(pin));
        users_.emplace(userId, user);
        upiToUserId_[upiId] = userId;
        phoneToUserId_[normalizedPhone] = userId;
        wallets_.emplace(userId, Wallet(userId, 0));
        return userId;
    }

    std::string linkBankAccount(const std::string& userId, const std::string& bankName,
                                const std::string& maskedNumber, double initialBalance) {
        User& user = getUserOrThrow(userId);
        const std::string accountId = "ACC_" + std::to_string(++accountCounter_);
        bankAccounts_.emplace(accountId,
                              BankAccount(accountId, user.getUserId(), bankName, maskedNumber,
                                          initialBalance));
        if (primaryAccountByUser_.count(userId) == 0) {
            primaryAccountByUser_[userId] = accountId;
        }
        return accountId;
    }

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

    void addBeneficiary(const std::string& userId, const std::string& nickname,
                        const std::string& upiId) {
        validateUser(userId);
        validateUpiId(upiId);
        beneficiaries_[userId].emplace_back(userId, nickname, upiId);
    }

    std::vector<Beneficiary> listBeneficiaries(const std::string& userId) const {
        validateUser(userId);
        auto it = beneficiaries_.find(userId);
        if (it == beneficiaries_.end()) {
            return {};
        }
        return it->second;
    }

    Transaction sendMoney(const std::string& payerUserId, const std::string& payeeUpiId,
                          double amount, const std::string& pin, PaymentRail rail,
                          const std::string& note = "",
                          const std::string& clientRequestId = "") {
        User& payer = getUserOrThrow(payerUserId);
        pinAuth_.verifyPin(payer, pin);
        const User& payee = resolveUserByUpi(payeeUpiId);
        return transferService_.execute(payer, payee, amount, rail, TransactionType::P2P_SEND,
                                        note, bankAccounts_, wallets_, processedClientIds_,
                                        clientRequestId, txnCounter_);
    }

    Transaction sendMoneyToPhone(const std::string& payerUserId, const std::string& payeePhone,
                                   double amount, const std::string& pin, PaymentRail rail,
                                   const std::string& note = "") {
        const std::string phone = normalizePhone(payeePhone);
        auto it = phoneToUserId_.find(phone);
        if (it == phoneToUserId_.end()) {
            throw std::runtime_error("payee phone not registered on GPay");
        }
        return sendMoney(payerUserId, users_.at(it->second).getUpiId(), amount, pin, rail, note);
    }

    Transaction scanAndPay(const std::string& payerUserId, const std::string& qrPayload,
                            const std::string& pin, PaymentRail rail) {
        User& payer = getUserOrThrow(payerUserId);
        pinAuth_.verifyPin(payer, pin);
        QrPaymentData qr = parseUpiQr(qrPayload);
        const User& payee = resolveUserByUpi(qr.payeeUpi);
        return transferService_.execute(payer, payee, qr.amount, rail, TransactionType::QR_PAY,
                                        qr.note, bankAccounts_, wallets_, processedClientIds_, "",
                                        txnCounter_);
    }

    std::string requestMoney(const std::string& requesterUserId, const std::string& payerUpiId,
                             double amount, const std::string& note) {
        const User& requester = getUserOrThrow(requesterUserId);
        const User& payer = resolveUserByUpi(payerUpiId);
        return requestMoneyService_.createRequest(requester, payer, amount, note, requestCounter_);
    }

    Transaction fulfillMoneyRequest(const std::string& payerUserId, const std::string& requestId,
                                    const std::string& pin, PaymentRail rail) {
        User& payer = getUserOrThrow(payerUserId);
        pinAuth_.verifyPin(payer, pin);

        MoneyRequest& request = requestMoneyService_.getRequestOrThrow(requestId);
        if (request.getPayerUserId() != payerUserId) {
            throw std::runtime_error("only payer can fulfill this request");
        }
        const User& requester = getUserOrThrow(request.getRequesterUserId());

        Transaction txn = transferService_.execute(
            payer, requester, request.getAmount(), rail, TransactionType::REQUEST_FULFILL,
            request.getNote(), bankAccounts_, wallets_, processedClientIds_, "", txnCounter_);
        requestMoneyService_.markFulfilled(requestId);
        return txn;
    }

    std::vector<Transaction> getTransactionHistory(const std::string& userId,
                                                   size_t limit = 20) const {
        validateUser(userId);
        return ledger_.listForUser(userId, limit);
    }

    const User& getUserByUpi(const std::string& upiId) const { return resolveUserByUpi(upiId); }

private:
    std::unordered_map<std::string, User> users_;
    std::unordered_map<std::string, std::string> upiToUserId_;
    std::unordered_map<std::string, std::string> phoneToUserId_;
    std::unordered_map<std::string, BankAccount> bankAccounts_;
    std::unordered_map<std::string, std::string> primaryAccountByUser_;
    std::unordered_map<std::string, Wallet> wallets_;
    std::unordered_map<std::string, std::vector<Beneficiary>> beneficiaries_;
    std::unordered_set<std::string> processedClientIds_;

    TransactionLedgerService ledger_;
    DailyLimitService limitService_;
    PinAuthService pinAuth_;
    RequestMoneyService requestMoneyService_;
    TransferService transferService_;

    int userCounter_{0};
    int accountCounter_{0};
    int txnCounter_{0};
    int requestCounter_{0};

    void validateUser(const std::string& userId) const {
        if (users_.find(userId) == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
    }

    User& getUserOrThrow(const std::string& userId) {
        auto it = users_.find(userId);
        if (it == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
        return it->second;
    }

    const User& getUserOrThrow(const std::string& userId) const {
        auto it = users_.find(userId);
        if (it == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
        return it->second;
    }

    const User& resolveUserByUpi(const std::string& upiId) const {
        validateUpiId(upiId);
        auto it = upiToUserId_.find(upiId);
        if (it == upiToUserId_.end()) {
            throw std::runtime_error("UPI ID not registered: " + upiId);
        }
        return users_.at(it->second);
    }

    const Wallet& getWalletOrThrow(const std::string& userId) const {
        auto it = wallets_.find(userId);
        if (it == wallets_.end()) {
            throw std::runtime_error("wallet not found");
        }
        return it->second;
    }

    const BankAccount& getPrimaryBankOrThrow(const std::string& userId) const {
        auto primaryIt = primaryAccountByUser_.find(userId);
        if (primaryIt == primaryAccountByUser_.end()) {
            throw std::runtime_error("no bank account linked");
        }
        auto accIt = bankAccounts_.find(primaryIt->second);
        if (accIt == bankAccounts_.end()) {
            throw std::runtime_error("bank account missing");
        }
        return accIt->second;
    }
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_CORE_GPAYSYSTEM_H
