// ============================================================================
//  services/MoneyManager.h  —  Paise ka hisaab (Requirement R6, R7)
// ----------------------------------------------------------------------------
//  "R6: Users can put cash into the machine."
//  "R7: The machine can add up how much money has been put in."
//
//  Poori class 3 function ki hai — aur yahi baat hai. Iska ek hi kaam hai:
//  "abhi machine me kitna paisa pada hai" ka hisaab rakhna.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Itni chhoti class alag file me kyun? — SINGLE RESPONSIBILITY         │
//  │                                                                          │
//  │  Ye class ko ye NAHI pata:                                               │
//  │    - product ka daam kya hai        (wo Slot/Item jaanta hai)            │
//  │    - paisa kaafi hai ya nahi        (wo MoneyInsertedState decide karta) │
//  │    - change kitna banega            (wo DispenseState nikaalta hai)      │
//  │                                                                          │
//  │  Wo bas jodta hai aur reset karta hai. Bas.                              │
//  │                                                                          │
//  │  Fayda: kal ko payment ka tareeka badle (UPI? card? coins ki ginti?), to │
//  │  sirf ye class badlegi. States, Inventory, Slot — kisi ko chhuna nahi    │
//  │  padega. Yahi requirements ka NFR #2 hai: "Payment and inventory modules │
//  │  should be decoupled."                                                   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ `double` se paisa rakhna — ek jaana-pehchana khatra:
//     Floating point me 0.1 + 0.2 exactly 0.3 nahi hota (0.30000000000000004
//     hota hai). Bade systems me isse paise ka hisaab bigad sakta hai.
//
//     Asli payment system me paisa **integer paise** me rakhte hain (₹40.50 ->
//     4050 paise) — tab koi rounding error hoti hi nahi. Yahan `double` isi
//     liye rakha hai kyunki demo hai aur amounts simple hain (40, 20, 50).
//     📌 Interview me ye baat khud se bolna bada plus point hai.
//
//  💡 `resetBalance()` kab bulaya jaata hai? Do jagah:
//       1. Product mil gaya (DispenseState) — paisa kharch ho gaya
//       2. Refund hua (R10, ya user ne cancel kiya) — paisa wapas mil gaya
//     Dono me balance 0 hona chahiye. `VendingMachine::resetTransaction()` isko
//     aur `clearSelectedSlot()` ko ek saath bulata hai — taaki ek bhi cheez
//     chhoot na jaye.
// ============================================================================
#ifndef VENDING_MACHINE_LLD_SERVICES_MONEYMANAGER_H
#define VENDING_MACHINE_LLD_SERVICES_MONEYMANAGER_H

#include <bits/stdc++.h>
#include <stdexcept>

using namespace std;

namespace vending_machine_lld {

class MoneyManager {
public:
    // ---- R6: cash daalo ----------------------------------------------------
    //
    // ⭐ `+=` hai, `=` nahi — kyunki R7 kehta hai machine JODTI hai. User ek
    // baar me 10 daale, phir 40 — balance 50 hona chahiye, 40 nahi.
    // (Base requirement #4 bhi yahi kehta hai: "User can insert money in
    //  multiple steps.")
    void insertMoney(double amount) {
        // Guard — 0 ya negative paisa ka koi matlab nahi. Negative allow karte
        // to user `insertMoney(-100)` bol ke machine se paisa NIKAAL leta! 😄
        // Chhota sa check, bada bachav.
        if (amount <= 0.0) {
            throw std::runtime_error("Inserted amount should be positive");
        }
        currentBalance_ += amount;
    }

    // ---- R7: ab tak kitna daala ---------------------------------------------
    double getCurrentBalance() const { return currentBalance_; }

    // ---- Transaction khatam — balance saaf (upar note padho) ----------------
    void resetBalance() { currentBalance_ = 0.0; }

private:
    // In-class initializer (`= 0.0`) — machine hamesha ₹0 se shuru hoti hai.
    // Constructor likhne ki zaroorat hi nahi padi.
    double currentBalance_ = 0.0;
};

} // namespace vending_machine_lld

#endif // VENDING_MACHINE_LLD_SERVICES_MONEYMANAGER_H
