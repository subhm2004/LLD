// ============================================================================
//  models/BankAccount.h  —  User ka linked bank account (balance ka asli maalik)
// ----------------------------------------------------------------------------
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ BALANCE PUBLIC NAHI HAI — sirf debit()/credit() se badalta hai      │
//  │                                                                          │
//  │  `setBalance()` jaan-boojh ke NAHI diya gaya. Agar hota to koi bhi        │
//  │  `account.setBalance(999999)` likh ke paisa bana leta — saare rules       │
//  │  bypass. Ye ENCAPSULATION ka sabse classic example hai, aur paise wale    │
//  │  code me ye optional nahi, ZAROORI hai.                                  │
//  │                                                                          │
//  │  Ab balance badalne ke sirf 2 darwaze hain, aur dono pe pehra hai:       │
//  │     debit()  -> amount > 0 AND balance >= amount   (warna throw)          │
//  │     credit() -> amount > 0                          (warna throw)         │
//  │                                                                          │
//  │  ⭐ `balance_ < amount` wala check YAHIN hai (service me nahi) — kyunki   │
//  │     ye INVARIANT hai: "balance kabhi negative nahi hoga". Invariant       │
//  │     hamesha usi class me enforce hona chahiye jo data ka maalik hai,      │
//  │     warna har naya caller check bhoolne ka risk laata hai.               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `maskedNumber_` ("XX1234") — poora account number kabhi store nahi karte.
//     Real app me ye PCI/RBI compliance ka rule hai: display ke liye sirf aakhri
//     4 digit. Data leak ho bhi jaye to poora number nahi milta.
//
//  ⭐ `debit` me `amount <= 0` bhi check hota hai — warna `debit(-500)` ka matlab
//     hota "balance BADHA do" (paisa chhaap liya 😱). Negative amount hamesha
//     block karo, chahe wo debit ho ya credit.
//
//  📌 `BankAccount() = default` chahiye kyunki ye `unordered_map` me value ki
//     tarah rehta hai — kuch map operations default-constructible type maangte hain.
// ============================================================================
#ifndef GPAY_LLD_MODELS_BANKACCOUNT_H
#define GPAY_LLD_MODELS_BANKACCOUNT_H

#include <stdexcept>
#include <string>
#include <utility>

namespace gpay_lld {

class BankAccount {
public:
    BankAccount() = default; // map me value ke liye

    BankAccount(std::string accountId, std::string userId, std::string bankName,
                std::string maskedNumber, double balance)
        : accountId_(std::move(accountId)),
          userId_(std::move(userId)),
          bankName_(std::move(bankName)),
          maskedNumber_(std::move(maskedNumber)), // ⭐ sirf "XX1234" (poora number nahi)
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
    // ⚠ setBalance() nahi hai — jaan-boojh ke (upar wala note padho)

    // ⭐ Paisa nikaalne ka EKLAUTA darwaza. Do pehre: positive amount + enough balance.
    void debit(double amount) {
        if (amount <= 0 || balance_ < amount) {
            throw std::runtime_error("insufficient bank balance");
        }
        balance_ -= amount;
    }

    // Paisa daalne ka darwaza. Negative credit = chhupa hua debit, isliye block.
    void credit(double amount) {
        if (amount <= 0) {
            throw std::invalid_argument("credit amount must be positive");
        }
        balance_ += amount;
    }

private:
    std::string accountId_;
    std::string userId_;      // kiska account hai
    std::string bankName_;
    std::string maskedNumber_;
    double balance_{0};       // ⭐ sirf debit/credit se badalta hai
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_MODELS_BANKACCOUNT_H
