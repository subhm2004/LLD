// ============================================================================
//  factories/PaymentRailFactory.h  —  enum se Strategy banane wali factory
// ----------------------------------------------------------------------------
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Factory + Strategy — hamesha jodi me                                  │
//  │     Strategy: "kaam KAISE hoga"                                          │
//  │     Factory:  "KAUNSA kaam wala object milega"                           │
//  │                                                                          │
//  │  Poore codebase me `new BankAccountRailStrategy` sirf YAHIN likha hai.    │
//  │  Isliye naya rail add karne pe sirf 2 jagah badalti hai:                 │
//  │     1. PaymentRail enum me nayi value                                    │
//  │     2. yahan ek `case`                                                    │
//  │  TransferService, GPaySystem, models — sab untouched. 📌 Yahi OCP hai.   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `switch` me `default:` NAHI likha — jaan-boojh ke!
//     Kal ko `PaymentRail` me nayi value add hui aur yahan case bhoola, to
//     compiler `-Wswitch` warning dega ("enumeration value not handled").
//     `default:` likh dete to compiler chup rehta aur bug RUNTIME pe milta. 😱
//     📌 Enum switch me default se bachna ek bahut kaam ka trick hai.
//     (Neeche wala `return` sirf isliye hai ki compiler "control reaches end of
//      non-void function" na bole — corrupted enum value ke liye safe fallback.)
//
//  ⭐ `shared_ptr` kyun (`unique_ptr` nahi)? Yahan `unique_ptr` bhi chal jaata —
//     strategy ek hi transfer me use hoti hai. `shared_ptr` isliye rakha hai ki
//     aage chal ke strategy CACHE karni ho (har transfer pe nayi banane ke bajaye)
//     to caller ka code badalna na pade. ⚠ Trade-off: shared_ptr me atomic
//     refcount ka thoda overhead hota hai.
//
//  📌 `static` method — factory ka apna koi state nahi, isliye object banane ki
//     zaroorat hi nahi. Ye "namespace of related functions" wala use hai.
// ============================================================================
#ifndef GPAY_LLD_FACTORIES_PAYMENTRAILFACTORY_H
#define GPAY_LLD_FACTORIES_PAYMENTRAILFACTORY_H

#include <memory>
#include <string>
#include <unordered_map>

#include "../enums/PaymentRail.h"
#include "../strategies/BankAccountRailStrategy.h"
#include "../strategies/IPaymentRailStrategy.h"
#include "../strategies/WalletRailStrategy.h"

namespace gpay_lld {

class PaymentRailFactory {
public:
    // ⭐ `primaryAccountByUser` ab pass hota hai — bank strategy ko pata hona
    //    chahiye ki user ka PRIMARY account kaunsa hai (pehle wo guess karta tha,
    //    jo ek bug tha — BankAccountRailStrategy.h ka note padho).
    static std::shared_ptr<IPaymentRailStrategy> create(
        PaymentRail rail,
        const std::unordered_map<std::string, std::string>& primaryAccountByUser) {
        switch (rail) { // ⭐ koi `default:` nahi (upar note)
            case PaymentRail::LINKED_BANK:
                return std::make_shared<BankAccountRailStrategy>(primaryAccountByUser);
            case PaymentRail::GPAY_WALLET:
                return std::make_shared<WalletRailStrategy>();
        }
        // Yahan tabhi pahunchenge jab enum value corrupt ho — safe fallback.
        return std::make_shared<BankAccountRailStrategy>(primaryAccountByUser);
    }
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_FACTORIES_PAYMENTRAILFACTORY_H
