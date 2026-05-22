#ifndef GPAY_LLD_MODELS_BANKACCOUNT_H
#define GPAY_LLD_MODELS_BANKACCOUNT_H

#include <stdexcept>
#include <string>

namespace gpay_lld {

class BankAccount {
public:
    BankAccount() = default;

    BankAccount(std::string accountId, std::string userId, std::string bankName,
                std::string maskedNumber, double balance)
        : accountId_(std::move(accountId)),
          userId_(std::move(userId)),
          bankName_(std::move(bankName)),
          maskedNumber_(std::move(maskedNumber)),
          balance_(balance) {
        if (balance < 0) {
            throw std::invalid_argument("balance cannot be negative");
        }
    }

    const std::string& getAccountId() const { return accountId_; }
    const std::string& getUserId() const { return userId_; }
    const std::string& getBankName() const { return bankName_; }
    const std::string& getMaskedNumber() const { return maskedNumber_; }
    double getBalance() const { return balance_; }

    void debit(double amount) {
        if (amount <= 0 || balance_ < amount) {
            throw std::runtime_error("insufficient bank balance");
        }
        balance_ -= amount;
    }

    void credit(double amount) {
        if (amount <= 0) {
            throw std::invalid_argument("credit amount must be positive");
        }
        balance_ += amount;
    }

private:
    std::string accountId_;
    std::string userId_;
    std::string bankName_;
    std::string maskedNumber_;
    double balance_{0};
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_MODELS_BANKACCOUNT_H
