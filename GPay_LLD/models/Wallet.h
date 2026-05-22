#ifndef GPAY_LLD_MODELS_WALLET_H
#define GPAY_LLD_MODELS_WALLET_H

#include <stdexcept>
#include <string>

namespace gpay_lld {

class Wallet {
public:
    Wallet() = default;

    Wallet(std::string userId, double balance) : userId_(std::move(userId)), balance_(balance) {
        if (balance < 0) {
            throw std::invalid_argument("wallet balance cannot be negative");
        }
    }

    const std::string& getUserId() const { return userId_; }
    double getBalance() const { return balance_; }

    void debit(double amount) {
        if (amount <= 0 || balance_ < amount) {
            throw std::runtime_error("insufficient wallet balance");
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
    std::string userId_;
    double balance_{0};
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_MODELS_WALLET_H
