// ============================================================================
//  models/Wallet.h  —  GPay ke andar ka balance (bank se alag)
// ----------------------------------------------------------------------------
//  BankAccount ka chhota bhai — bilkul wahi encapsulation, wahi debit/credit
//  guards. Farak sirf itna ki iska maalik BANK nahi, APP hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Wallet aur BankAccount alag class kyun? (dono me balance hi to hai)   │
//  │                                                                          │
//  │  Kyunki inka MATLAB alag hai, sirf data same hai:                        │
//  │     BankAccount -> paisa BANK ke paas hai; app sirf request bhejta hai    │
//  │                    (accountId, bankName, masked number sab chahiye)       │
//  │     Wallet      -> paisa APP ke paas hai; ek user ka ek hi wallet         │
//  │                    (na bank ka naam, na account number)                   │
//  │                                                                          │
//  │  Aur lookup bhi alag hai: wallets ka key seedha `userId` hai, jabki       │
//  │  bank accounts ka key `accountId` (ek user ke KAI accounts ho sakte hain).│
//  │  📌 Ek hi "Account" class bana ke `type` field daal dete to har jagah     │
//  │     `if (type == WALLET)` likhna padta — wahi to Strategy pattern se      │
//  │     bachna tha.                                                          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Constructor me `balance < 0` check — wallet negative se shuru nahi ho sakta.
//     Aur `debit` me `balance_ < amount` — yaani wallet kabhi overdraft me nahi
//     jaata. Ye INVARIANT class ke andar enforce hai (upar BankAccount ka note).
// ============================================================================
#ifndef GPAY_LLD_MODELS_WALLET_H
#define GPAY_LLD_MODELS_WALLET_H

#include <stdexcept>
#include <string>
#include <utility>

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

    // Wahi do pehre: positive amount + enough balance (overdraft nahi).
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
    std::string userId_; // ⭐ ek user = ek wallet (isliye map ka key bhi userId hai)
    double balance_{0};
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_MODELS_WALLET_H
