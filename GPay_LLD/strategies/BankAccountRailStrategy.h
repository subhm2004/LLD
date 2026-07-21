// ============================================================================
//  strategies/BankAccountRailStrategy.h  —  Bank account se paisa (asli UPI rail)
// ----------------------------------------------------------------------------
//  Kaam: user ka PRIMARY linked bank account dhundo, usme se debit/credit karo.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  🐛 BUG FIX: pehle GALAT account se paisa katta tha                       │
//  │                                                                          │
//  │  PEHLE `getPrimaryAccount` aisa tha:                                      │
//  │      for (auto& e : bankAccounts)                                         │
//  │          if (e.second.getUserId() == userId) return e.second;  // ⚠      │
//  │                                                                          │
//  │  Yaani "us user ka JO PEHLA account scan me mile". Do problem:            │
//  │   1. `unordered_map` ka traversal order NON-DETERMINISTIC hai — do        │
//  │      accounts wale user ka paisa kabhi HDFC se katta, kabhi SBI se.       │
//  │      Har run me alag! 😱                                                  │
//  │   2. GPaySystem alag se `primaryAccountByUser_` maintain karta hai, aur   │
//  │      `getPrimaryBankBalance()` USI se balance dikhata hai. Matlab UI      │
//  │      HDFC ka balance dikhata tha par paisa SBI se kat jaata tha.          │
//  │                                                                          │
//  │  CONFIRM kiya tha: 2 accounts (ACC_1 primary, ACC_2) wale user se ₹100    │
//  │  bheja -> primary balance 1000 ka 1000 hi raha (doosre se kata). ✅ bug   │
//  │                                                                          │
//  │  FIX: strategy ko ab `primaryAccountByUser` map diya jaata hai (constructor│
//  │  se). Ab wo GUESS nahi karti — jo account system ne primary bola hai,     │
//  │  bilkul wahi use hota hai. Deterministic aur UI ke saath consistent.      │
//  │                                                                          │
//  │  📌 Purana dead field `defaultAccountId_` bhi hata diya — wo constructor  │
//  │     me leta tha par KABHI use nahi hota tha (factory usme `""` bhejta     │
//  │     tha). Aisa dead code sabse dhokebaaz hota hai: padhne wale ko lagta   │
//  │     hai account select ho raha hai, jabki ho kuch aur raha hota hai.      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Strategy ne khud koi balance rule nahi likha — `account.debit(amount)`
//     bulaya aur bas. "Balance kaafi hai kya" ka check `BankAccount` ke andar
//     hai, kyunki wahi balance ka MAALIK hai. Strategy sirf ye decide karti hai
//     ki KAUNSA account (routing), na ki paisa kaise ghatega (rules).
//
//  ⚠ `primaryAccounts_` ek REFERENCE member hai — GPaySystem ka map. Iska matlab
//     GPaySystem is strategy se zyada jeena chahiye. Yahan safe hai kyunki
//     strategy har transfer pe banti hai aur usi call me khatam ho jaati hai.
// ============================================================================
#ifndef GPAY_LLD_STRATEGIES_BANKACCOUNTRAILSTRATEGY_H
#define GPAY_LLD_STRATEGIES_BANKACCOUNTRAILSTRATEGY_H

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "IPaymentRailStrategy.h"

namespace gpay_lld {

class BankAccountRailStrategy : public IPaymentRailStrategy {
public:
    // ⭐ Primary-account map INJECTED (pehle yahan bekaar ka defaultAccountId tha).
    explicit BankAccountRailStrategy(
        const std::unordered_map<std::string, std::string>& primaryAccountByUser)
        : primaryAccounts_(primaryAccountByUser) {}

    PaymentRail railType() const override { return PaymentRail::LINKED_BANK; }

    void debit(const std::string& userId, double amount,
               std::unordered_map<std::string, BankAccount>& bankAccounts,
               std::unordered_map<std::string, Wallet>& /*wallets*/) override {
        BankAccount& account = getPrimaryAccount(userId, bankAccounts);
        account.debit(amount); // ⭐ balance rule BankAccount ke andar hai
    }

    void credit(const std::string& userId, double amount,
                std::unordered_map<std::string, BankAccount>& bankAccounts,
                std::unordered_map<std::string, Wallet>& /*wallets*/) override {
        BankAccount& account = getPrimaryAccount(userId, bankAccounts);
        account.credit(amount);
    }

private:
    // ⚠ Reference member — GPaySystem ka map (lifetime ka note upar).
    const std::unordered_map<std::string, std::string>& primaryAccounts_;

    // ⭐ FIXED: scan/guess nahi — system ka declared primary account.
    BankAccount& getPrimaryAccount(
        const std::string& userId,
        std::unordered_map<std::string, BankAccount>& bankAccounts) const {
        auto primaryIt = primaryAccounts_.find(userId);
        if (primaryIt == primaryAccounts_.end()) {
            throw std::runtime_error("no linked bank account for user: " + userId);
        }
        auto accountIt = bankAccounts.find(primaryIt->second);
        if (accountIt == bankAccounts.end()) {
            // Data inconsistency: primary id hai par account gayab.
            throw std::runtime_error("primary bank account missing for user: " + userId);
        }
        return accountIt->second;
    }
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_STRATEGIES_BANKACCOUNTRAILSTRATEGY_H
