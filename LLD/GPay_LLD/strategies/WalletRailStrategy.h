// ============================================================================
//  strategies/WalletRailStrategy.h  —  GPay wallet balance se paisa
// ----------------------------------------------------------------------------
//  Bank wale rail ka jodidaar. Same interface, bilkul alag andar ka kaam —
//  yahi Strategy pattern ka poora point hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ debit aur credit ka behavior JAAN-BOOJH KE alag hai                   │
//  │                                                                          │
//  │     debit  -> wallet nahi mila -> THROW                                   │
//  │     credit -> wallet nahi mila -> khaali wallet BANA do, phir credit       │
//  │                                                                          │
//  │  Ye galti nahi, soch-samajh ke rakha gaya farak hai:                      │
//  │   • Paisa NIKAALNA hai aur wallet hi nahi -> yakeenan kuch galat hai      │
//  │     (registration me wallet ban jaata hai). Chup-chaap khaali wallet      │
//  │     bana ke "insufficient balance" bolna bug ko chhupa dega. Isliye throw.│
//  │   • Paisa DAALNA hai aur wallet nahi -> koi harm nahi. Wallet bana do     │
//  │     aur paisa daal do. User ka paisa isliye nahi rukna chahiye ki uska    │
//  │     wallet record abhi bana nahi. 📌 "Be strict on the way out,           │
//  │     forgiving on the way in."                                            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Ye class bank wale se KAAFI chhoti hai — wallet ka lookup seedha `userId`
//     se hota hai (ek user = ek wallet). Bank me ek user ke kai accounts ho
//     sakte hain, isliye wahan "primary" ka poora concept chahiye tha.
//
//  📌 `bankAccounts` param ka naam nahi likha (`/*bankAccounts*/`) — wallet rail
//     ko usse matlab hi nahi. Isse `-Wunused-parameter` warning bhi nahi aati
//     aur padhne wale ko turant dikhta hai ki ye param yahan bekaar hai.
// ============================================================================
#ifndef GPAY_LLD_STRATEGIES_WALLETRAILSTRATEGY_H
#define GPAY_LLD_STRATEGIES_WALLETRAILSTRATEGY_H

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "IPaymentRailStrategy.h"

namespace gpay_lld {

class WalletRailStrategy : public IPaymentRailStrategy {
public:
    PaymentRail railType() const override { return PaymentRail::GPAY_WALLET; }

    // ⭐ Wallet nahi mila -> THROW (chhupao mat — kuch to gadbad hai).
    void debit(const std::string& userId, double amount,
               std::unordered_map<std::string, BankAccount>& /*bankAccounts*/,
               std::unordered_map<std::string, Wallet>& wallets) override {
        auto it = wallets.find(userId);
        if (it == wallets.end()) {
            throw std::runtime_error("wallet not found for user: " + userId);
        }
        it->second.debit(amount); // balance ka rule Wallet ke andar
    }

    // ⭐ Wallet nahi mila -> bana do (paisa aa raha hai, rokna kyun).
    void credit(const std::string& userId, double amount,
                std::unordered_map<std::string, BankAccount>& /*bankAccounts*/,
                std::unordered_map<std::string, Wallet>& wallets) override {
        // emplace iterator + bool deta hai: naya bana to iterator naye ka,
        // pehle se tha to purane ka. Dono case ek hi line me handle.
        auto result = wallets.emplace(userId, Wallet(userId, 0));
        result.first->second.credit(amount);
    }
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_STRATEGIES_WALLETRAILSTRATEGY_H
