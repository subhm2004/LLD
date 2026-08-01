// ============================================================================
//  strategies/IPaymentRailStrategy.h  —  ⭐ STRATEGY interface (rail ka contract)
// ----------------------------------------------------------------------------
//  "Paisa kahan se kaatna hai aur kahan daalna hai" — ye poora sawaal is ek
//  interface ke peeche chhup jaata hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Strategy Pattern ka asli fayda yahan                                  │
//  │                                                                          │
//  │  `TransferService` sirf ye jaanta hai:                                    │
//  │       strategy->debit(payer, amount, ...)                                 │
//  │       strategy->credit(payee, amount, ...)                                │
//  │                                                                          │
//  │  Usse zero matlab hai ki andar bank account dhunda ja raha hai ya wallet. │
//  │  Naya rail (CREDIT_LINE, RUPAY_CC, FOREX) add karna ho to:                │
//  │       nayi class : public IPaymentRailStrategy + factory me ek case       │
//  │  TransferService ki ek line bhi nahi badalti. (Open/Closed Principle)     │
//  │                                                                          │
//  │  Bina Strategy ke TransferService me ye hota:                             │
//  │       if (rail == BANK) { ...20 lines... }                                │
//  │       else if (rail == WALLET) { ...20 lines... }                         │
//  │  ...aur har naye rail pe wahi function bada hota jaata, aur har badlaav   │
//  │  purane rails ko todne ka risk laata.                                    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `virtual ~IPaymentRailStrategy() = default;` — ye line MISS mat karna!
//     Object `shared_ptr<IPaymentRailStrategy>` ke through delete hota hai.
//     Agar destructor virtual na hota to base ka destructor chalta, derived ka
//     NAHI — resource leak + undefined behavior. 📌 RULE: jis class ka koi
//     virtual function hai, uska destructor bhi virtual hona chahiye.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠ Design smell (honest note): debit/credit ko poore MAPS pass hote hain  │
//  │                                                                          │
//  │  Strategy ko poora `bankAccounts` aur `wallets` map mil jaata hai —       │
//  │  yaani wo kisi ka bhi paisa chhoo sakta hai. Behtar design me ek chhota   │
//  │  `IAccountResolver` hota jo sirf "is user ka account do" deta.            │
//  │  Chhote LLD me ye simplification chalti hai, par interview me khud bolna  │
//  │  ki tumhe iska pata hai — ye samajh dikhata hai.                          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Har concrete strategy ek map IGNORE karti hai (bank wale ko wallets se
//     kaam nahi). Isliye unused parameters ka NAAM nahi likha jaata —
//     `std::unordered_map<...>&` (bina naam ke) — isse `-Wunused-parameter`
//     warning nahi aati aur intent bhi saaf dikhta hai.
// ============================================================================
#ifndef GPAY_LLD_STRATEGIES_IPAYMENTRAILSTRATEGY_H
#define GPAY_LLD_STRATEGIES_IPAYMENTRAILSTRATEGY_H

#include <string>
#include <unordered_map>

#include "../enums/PaymentRail.h"
#include "../models/BankAccount.h"
#include "../models/Wallet.h"

namespace gpay_lld {

class IPaymentRailStrategy {
public:
    virtual ~IPaymentRailStrategy() = default; // ⭐ virtual (upar note)

    // Kaunsa rail hai — Transaction record me store hota hai.
    virtual PaymentRail railType() const = 0;

    // Paisa KAATO (fail ho to throw — kabhi silent na ho).
    virtual void debit(const std::string& userId, double amount,
                       std::unordered_map<std::string, BankAccount>& bankAccounts,
                       std::unordered_map<std::string, Wallet>& wallets) = 0;

    // Paisa DAALO.
    virtual void credit(const std::string& userId, double amount,
                        std::unordered_map<std::string, BankAccount>& bankAccounts,
                        std::unordered_map<std::string, Wallet>& wallets) = 0;
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_STRATEGIES_IPAYMENTRAILSTRATEGY_H
